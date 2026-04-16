#include "display/views/cal_view.h"


struct text* calibration_text_ptr = &Text1;
struct text* percentage_text_ptr = &Text2;

struct flag_manager* cal_compass_flags = create_flag_manager(NUM_CAL_COMPASS_FLAGS);

struct compass_needle_pos calibration_circle_positions[NUM_PIXELS_CALIBRATION_CIRCLE];

// The pixels corresponding to the portions of the calibration circle that we still want to calibrate.
struct compass_needle_pos* compass_portions[NUM_COMPASS_PORTIONS][NUM_PIXELS_CALIBRATION_CIRCLE / NUM_COMPASS_PORTIONS + 1]; // +1 to be safe in case of rounding issues
uint8_t compass_portions_sizes[NUM_COMPASS_PORTIONS] = {0};
uint8_t calibration_points_left[NUM_COMPASS_PORTIONS] = {0};
uint16_t num_calibration_points_done = 0;




void enter_cal_view() {
    Compass.update = cal_compass_update;
    Compass.flags = cal_compass_flags; //this attributes 'flags' is used in the warn function
    change_text_message(calibration_text_ptr, (char *)"Calibrage");
    change_text_position(calibration_text_ptr, 2, 0);
    display_refresh_text(calibration_text_ptr);

    change_text_message(percentage_text_ptr, (char *)" 0%");
    change_text_position(percentage_text_ptr, 5, 1);
    display_refresh_text(percentage_text_ptr);

    Compass.on_enter();
    active_view_idx = CALIBRATION_VIEW;


    // reset the min and max values for the magnetometer calibration
    magnetometer_calibrate_reset();
    num_calibration_points_done = 0;
    // Initialize calibration circle positions to (0,0)
    for(uint8_t i = 0; i < NUM_PIXELS_CALIBRATION_CIRCLE; i++){
        calibration_circle_positions[i].x = 0;
        calibration_circle_positions[i].y = 0;
    }

    for(uint8_t i = 0; i < NUM_COMPASS_PORTIONS; i++){
        compass_portions_sizes[i] = 0;
        calibration_points_left[i] = NUM_CALIBRATION_POINTS_PER_PORTION;
    }

    // Compute the positions of the pixels on the calibration circle and assign them to compass portions
   int count = 0;

    for(int i = 0; i < NUM_PIXELS_CALIBRATION_CIRCLE; i++){
        double theta = 2 * M_PI * i / NUM_PIXELS_CALIBRATION_CIRCLE;

        int x = (int)CX + (int)round((NEEDLE_LENGTH - 1.0) * cos(theta));
        int y = (int)CY - (int)round((NEEDLE_LENGTH - 1.0) * sin(theta));
        if(x >= 0 && x < W_COMPASS && y >= 0 && y < H_COMPASS){
            
            calibration_circle_positions[count].x = x;
            calibration_circle_positions[count].y = y;

            int idx_portion = (int)(theta * NUM_COMPASS_PORTIONS / (2 * M_PI));

            if(idx_portion >= NUM_COMPASS_PORTIONS)
                idx_portion = NUM_COMPASS_PORTIONS - 1;

            if (compass_portions_sizes[idx_portion] < (NUM_PIXELS_CALIBRATION_CIRCLE / NUM_COMPASS_PORTIONS + 1)) {
                compass_portions[idx_portion][compass_portions_sizes[idx_portion]] =
                    &calibration_circle_positions[count];

                compass_portions_sizes[idx_portion]++;
            }

            count++;
        }

    }
}

void update_cal_view(){
    Compass.update();
}

void cal_compass_update() {
    struct flag_manager* flags = Compass.flags;
    if(update_magnetometer_bearing()){
        set_flag(flags, CAL_CHANGED_MAGNETOMETER_BEARING);
    }

    if (is_flag_set(flags, CAL_CHANGED_MAGNETOMETER_BEARING)) {
        if (cal_update_needle_bearing()) {
            set_flag(flags, CAL_CHANGED_NEEDLE_BEARING);
        }
    }

    if (is_flag_set(flags, CAL_CHANGED_NEEDLE_BEARING)){
        uint8_t i = bearing_to_portion_index();
        if(calibration_points_left[i] != 0){
            calibration_points_left[i]--;
            if(calibration_points_left[i] == 0) {
                set_flag(flags, CAL_CHANGED_PORTIONS);
            }
            magnetometer_calibrate_aquire();
            num_calibration_points_done++;
            uint8_t percentage = (num_calibration_points_done * 100) / (NUM_COMPASS_PORTIONS * NUM_CALIBRATION_POINTS_PER_PORTION);
            char percentage_message[6];
            snprintf(percentage_message, 6, "%3d%%", percentage);
            change_text_message(percentage_text_ptr, percentage_message);
        }
        if(update_needle_position()){
            set_flag(flags, CAL_CHANGED_NEEDLE_POSITION);
        }
    }


    if (is_flag_set(flags, CAL_CHANGED_NEEDLE_POSITION) || is_flag_set(flags, CAL_CHANGED_PORTIONS)) {
        clear_whole_compass();
        compass_grid_draw_portions();
        compass_grid_draw_needle();
        set_flag(flags, CAL_CHANGED_COMPASS_GRID);
    }   

    if (is_flag_set(flags, CAL_CHANGED_COMPASS_GRID)) {
        display_refresh_compass();
    }
    
    if (is_flag_set(percentage_text_ptr->component.flags, CHANGED_TEXT)) {
        display_refresh_text(percentage_text_ptr);
        clear_flag(percentage_text_ptr->component.flags, CHANGED_TEXT);
    }
    for (int i = 0; i < NUM_COMPASS_PORTIONS; i++){
    }
    clear_all_flags(flags);

    magnetometer_calibrate_print();

}

void compass_grid_draw_portions(){

    for(uint8_t i = 0; i < NUM_COMPASS_PORTIONS; i++){
        if (calibration_points_left[i] != 0) {
            for(uint8_t j = 0; j < compass_portions_sizes[i]; j++){
                set_pixel_bin_matrix(compass_grid, compass_portions[i][j]->x, compass_portions[i][j]->y, 1);
            }   
        }
    }
}

uint8_t cal_update_needle_bearing(){
    // needle_bearing = M_PI_2 - magnetometer_bearing;
    needle_bearing = magnetometer_bearing; // we want the needle to point in the direction of the magnetic field, not the opposite
    return 1;
}

uint8_t bearing_to_portion_index() {
    int idx = (int)(needle_bearing * NUM_COMPASS_PORTIONS / (2.0 * M_PI));
    if (idx >= NUM_COMPASS_PORTIONS) {
        idx = NUM_COMPASS_PORTIONS - 1;
    }
    return (uint8_t)idx;
}
