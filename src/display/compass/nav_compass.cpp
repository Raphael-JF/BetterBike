#include <math.h>
#include <stdint.h>


#include "display/compass/nav_compass.h"



blinking compass_frame_blinking = blinking_create(500, 0) ;


struct component Nav_compass = {
    nav_compass_component_on_enter,
    nav_compass_component_update,
    create_flag_manager(NUM_NAV_COMPASS_FLAGS)
};


void nav_compass_component_on_enter() {
    lcd.setCursor(13, 0);
    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.setCursor(13, 1);
    lcd.write(byte(3));
    lcd.write(byte(4));
    lcd.write(byte(5));

    clear_whole_compass();
    update_current_position();
    update_waypoint_bearing();
    update_magnetometer_bearing();
    update_bearing_to_display();
    calculate_nav_compass_grid();
    display_refresh_compass();
}

void nav_compass_component_update() {
    flag_manager* flags = Nav_compass.flags;
    if(is_flag_set(flags, CHANGED_CURRENT_POSITION)  || is_flag_set(flags, CHANGED_WAYPOINT_POSITION)){
        if(update_waypoint_bearing()){
            set_flag(flags, CHANGED_WAYPOINT_BEARING);
        }
    }

    if (is_flag_set(flags, CHANGED_WAYPOINT_BEARING) || is_flag_set(flags, CHANGED_MAGNETOMETER_BEARING)){
        if(update_bearing_to_display()){
            set_flag(flags, CHANGED_BEARING_TO_DISPLAY);
        }
    }

    if (is_flag_set(flags, CHANGED_BEARING_TO_DISPLAY)){
        if (calculate_nav_compass_grid()) {

            set_flag(flags, CHANGED_COMPASS_GRID);
        }
    }

    if (is_flag_set(flags, DO_HIGHLIGHT_FRAME) && is_flag_set(flags, DO_UNHIGHLIGHT_FRAME)) {
        Serial.println("Warning: both highlight and unhighlight frame flags are set at the same time. This should not happen.");
    }
    
    if (is_flag_set(flags, DO_HIGHLIGHT_FRAME)) {
        highlight_compass_frame();
        set_flag(flags, CHANGED_COMPASS_GRID);
    }

    if (is_flag_set(flags, DO_UNHIGHLIGHT_FRAME)) {
        unhighlight_compass_frame();
        set_flag(flags, CHANGED_COMPASS_GRID);
    }

    if (is_flag_set(flags, CHANGED_COMPASS_GRID)) {

        display_refresh_compass();
    }

    clear_all_flags(flags);
}


enum gps_timeout_status_transition update_gps_timeout_status() {
    unsigned long current_millis = millis();
    switch (timeout_status) {
        case GPS_TIMEOUT_STATUS_OK:
            if (current_millis - last_gps_sync_millis > GPS_TIMEOUT_OLD){
                timeout_status = GPS_TIMEOUT_STATUS_OLD;
                return OK_TO_OLD;
            }
            break;
        case GPS_TIMEOUT_STATUS_OLD:
            if (current_millis - last_gps_sync_millis < GPS_TIMEOUT_OLD) {
                timeout_status = GPS_TIMEOUT_STATUS_OK;
                return OLD_TO_OK;
            }
            if (current_millis - last_gps_sync_millis > GPS_TIMEOUT_INVALID) {
                timeout_status = GPS_TIMEOUT_STATUS_INVALID;
                return OLD_TO_INVALID;
            }
            break;
        case GPS_TIMEOUT_STATUS_INVALID:
            if (current_millis - last_gps_sync_millis < GPS_TIMEOUT_OLD) {
                timeout_status = GPS_TIMEOUT_STATUS_OK;
                return INVALID_TO_OK;
            }
            break;
    }
    return NO_TRANSITION;
}



/*
    Dessine une ligne de Bresenham entre les points (x0, y0) et (x1, y1) sur compass_grid. Les points doivent être dans les limites de la grille.
*/
void draw_line(int x0, int y0, int x1, int y1) {
    int dx = fabs(x1 - x0);
    int dy = fabs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1) {
        // allume pixel
        if (x0 >= 0 && x0 < W_gps && y0 >= 0 && y0 < H_gps)
            set_pixel_bin_matrix(compass_grid, x0, y0, true);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}


uint8_t calculate_nav_compass_grid(){
    double math_compass_angle = M_PI_2 - bearing_to_display;

    math_compass_angle = fmod(math_compass_angle, 2 * M_PI);
    if (math_compass_angle < 0) {
        math_compass_angle += 2 * M_PI;
    }

    double dx = cos(math_compass_angle);
    double dy = -sin(math_compass_angle);

    uint8_t x = CX + round((double)(ARROW_LENGTH)*dx);
    uint8_t y = CY + round((double)(ARROW_LENGTH)*dy);

    if(x == compass_pos.x && y == compass_pos.y) {
        return 0;
    }

    clear_inner_compass();

    compass_pos.x = x;
    compass_pos.y = y;
    uint8_t cx = (uint8_t)CX;
    uint8_t cy = (uint8_t)CY;


    if(math_compass_angle < M_PI / 2) {
        draw_line(cx, cy - 1, x, y - 1);
    }
    else if(math_compass_angle < M_PI) {
        draw_line(cx - 1, cy - 1, x - 1, y - 1);

    }
    else if(math_compass_angle < 3*M_PI/2) {
        draw_line(cx - 1, cy, x - 1, y);
    }
    else{
        draw_line(cx, cy, x, y);
    }
    return 1;
}

