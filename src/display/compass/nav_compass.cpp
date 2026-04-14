#include <math.h>
#include <stdint.h>


#include "display/compass/nav_compass.h"



blinking compass_frame_blinking = blinking_create(500, 0) ;


struct component Nav_compass = {
    nav_compass_component_on_enter,
    nav_compass_update,
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
    nav_update_bearing_to_display();
    update_needle_position();
    compass_grid_draw_needle();
    display_refresh_compass();
}

void nav_compass_update() {
    flag_manager* flags = Nav_compass.flags;
    if(is_flag_set(flags, NAV_CHANGED_CURRENT_POSITION)  || is_flag_set(flags, NAV_CHANGED_WAYPOINT_POSITION)){
        if(update_waypoint_bearing()){
            set_flag(flags, NAV_CHANGED_WAYPOINT_BEARING);
        }
    }

    if (is_flag_set(flags, NAV_CHANGED_MAGNETOMETER_RAW_DATA)) {
        if(update_magnetometer_bearing()){
            set_flag(flags, NAV_CHANGED_MAGNETOMETER_BEARING);
        }
    }

    if (is_flag_set(flags, NAV_CHANGED_WAYPOINT_BEARING) || is_flag_set(flags, NAV_CHANGED_MAGNETOMETER_BEARING)){
        if(nav_update_bearing_to_display()){
            set_flag(flags, NAV_CHANGED_BEARING_TO_DISPLAY);
        }
    }

    if (is_flag_set(flags, NAV_CHANGED_BEARING_TO_DISPLAY)){
        if(update_needle_position()){
            set_flag(flags, NAV_CHANGED_NEEDLE_POSITION);
        }
    }

    if(is_flag_set(flags, NAV_CHANGED_NEEDLE_POSITION)){
        clear_inner_compass_grid();
        compass_grid_draw_needle();
        set_flag(flags, NAV_CHANGED_COMPASS_GRID);
    }

    if (is_flag_set(flags, NAV_DO_HIGHLIGHT_FRAME) && is_flag_set(flags, NAV_DO_UNHIGHLIGHT_FRAME)) {
        Serial.println("Warning: both highlight and unhighlight frame flags are set at the same time. This should not happen.");
    }
    
    if (is_flag_set(flags, NAV_DO_HIGHLIGHT_FRAME)) {
        highlight_compass_frame();
        set_flag(flags, NAV_CHANGED_COMPASS_GRID);
    }

    if (is_flag_set(flags, NAV_DO_UNHIGHLIGHT_FRAME)) {
        unhighlight_compass_frame();
        set_flag(flags, NAV_CHANGED_COMPASS_GRID);
    }

    if (is_flag_set(flags, NAV_CHANGED_COMPASS_GRID)) {

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



uint8_t nav_update_bearing_to_display(){
    double new_bearing = waypoint_bearing - magnetometer_bearing ; 
    // Normaliser l'angle entre 0 et 2*PI
    new_bearing = fmod(new_bearing, 2 * M_PI);
    if (new_bearing < 0) {
        new_bearing += 2 * M_PI;
    }
    if(fabs(new_bearing - bearing_to_display) < 0.05){
        return 0;
    }
    bearing_to_display = new_bearing;
    return 1;
}