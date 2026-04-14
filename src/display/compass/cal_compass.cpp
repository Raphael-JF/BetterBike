#include "cal_compass.h"


struct compass_needle_pos calibration_circle_positions[NUM_PIXELS_CALIBRATION_CIRCLE];

// The pixels corresponding to the portions of the calibration circle that we still want to calibrate.
struct compass_needle_pos* compass_portions[NUM_COMPASS_PORTIONS][NUM_PIXELS_CALIBRATION_CIRCLE / NUM_COMPASS_PORTIONS + 1]; // +1 to be safe in case of rounding issues
uint8_t compass_portions_sizes[NUM_COMPASS_PORTIONS] = {0};
uint8_t calibration_points_left[NUM_COMPASS_PORTIONS] = {0};

static uint8_t bearing_to_portion_index(double bearing) {
    double normalized = fmod(bearing, 2.0 * M_PI);
    if (normalized < 0.0) {
        normalized += 2.0 * M_PI;
    }

    int idx = (int)(normalized * NUM_COMPASS_PORTIONS / (2.0 * M_PI));
    if (idx >= NUM_COMPASS_PORTIONS) {
        idx = NUM_COMPASS_PORTIONS - 1;
    }
    return (uint8_t)idx;
}


struct component Cal_compass = {
    cal_compass_on_enter,
    cal_compass_update,
    create_flag_manager(NUM_CAL_COMPASS_FLAGS)
};


void cal_compass_on_enter() {
    lcd.setCursor(13, 0);
    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.setCursor(13, 1);
    lcd.write(byte(3));
    lcd.write(byte(4));
    lcd.write(byte(5));


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
    clear_whole_compass();
    compass_grid_draw_portions();
    display_refresh_compass();
}

void cal_compass_update() {
    struct flag_manager* flags = Cal_compass.flags;
    if(is_flag_set(flags, CAL_CHANGED_MAGNETOMETER_RAW_DATA)){
        magnetometer_calibrate_aquire();
    }
    if(update_magnetometer_bearing()){
        set_flag(flags, CAL_CHANGED_MAGNETOMETER_BEARING);
    }

    if (is_flag_set(flags, CAL_CHANGED_MAGNETOMETER_BEARING)) {
        if (cal_update_bearing_to_display()) {
            set_flag(flags, CAL_CHANGED_BEARING_TO_DISPLAY);
        }
    }

    if (is_flag_set(flags, CAL_CHANGED_BEARING_TO_DISPLAY)){
        if(update_needle_position()){
            set_flag(flags, CAL_CHANGED_NEEDLE_POSITION);
        }
    }

    if (is_flag_set(flags, CAL_CHANGED_MAGNETOMETER_RAW_DATA)) {
        uint8_t i = bearing_to_portion_index(bearing_to_display);
        if(calibration_points_left[i] != 0){
            calibration_points_left[i]--;
            set_flag(flags, CAL_CHANGED_PORTIONS);
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
    
    print_flags(flags, NUM_CAL_COMPASS_FLAGS);

    clear_all_flags(flags);
}

void compass_grid_draw_portions(){

    for(uint8_t i = 0; i < NUM_COMPASS_PORTIONS; i++){
        if (calibration_points_left[i] > 0) {
            for(uint8_t j = 0; j < compass_portions_sizes[i]; j++){
                set_pixel_bin_matrix(compass_grid, compass_portions[i][j]->x, compass_portions[i][j]->y, 1);
            }   
        }
    }
}

uint8_t cal_update_bearing_to_display(){
    bearing_to_display = magnetometer_bearing;
    return 1;
}