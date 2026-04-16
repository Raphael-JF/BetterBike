#include "display/views/gps_view.h"


blinking gps_compass_frame_blinking = blinking_create(500, 0) ;
struct flag_manager* gps_compass_flags = create_flag_manager(NUM_GPS_COMPASS_FLAGS);


void enter_gps_view(){
    Compass.update = gps_compass_update;
    Compass.flags = gps_compass_flags; //this attributes 'flags' is used in the warn function


    Compass.on_enter(); // just bind the 6 cars
    Clock.on_enter(); // just print --:-- at the beginning
    active_view_idx = GPS_VIEW;
}

void update_gps_view(){
    Compass.update();
    Clock.update();
}

void gps_compass_update(){
    flag_manager* flags = Compass.flags;
    if(is_flag_set(flags,GPS_CHANGED_CURRENT_POSITION)  || is_flag_set(flags,GPS_CHANGED_WAYPOINT_POSITION)){
        if(update_waypoint_bearing()){
            set_flag(flags,GPS_CHANGED_WAYPOINT_BEARING);
        }
    }

    if (is_flag_set(flags,GPS_CHANGED_MAGNETOMETER_RAW_DATA)) {
        if(update_magnetometer_bearing()){
            set_flag(flags,GPS_CHANGED_MAGNETOMETER_BEARING);
        }
    }

    if (is_flag_set(flags,GPS_CHANGED_WAYPOINT_BEARING) || is_flag_set(flags,GPS_CHANGED_MAGNETOMETER_BEARING)){
        if(gps_update_needle_bearing()){
            set_flag(flags,GPS_CHANGED_NEEDLE_BEARING);
        }
    }

    if (is_flag_set(flags,GPS_CHANGED_NEEDLE_BEARING)){
        if(update_needle_position()){
            set_flag(flags,GPS_CHANGED_NEEDLE_POSITION);
        }
    }

    if(is_flag_set(flags,GPS_CHANGED_NEEDLE_POSITION)){
        clear_inner_compass_grid();
        compass_grid_draw_needle();
        set_flag(flags,GPS_CHANGED_COMPASS_GRID);
    }

    if (is_flag_set(flags,GPS_DO_HIGHLIGHT_FRAME) && is_flag_set(flags,GPS_DO_UNHIGHLIGHT_FRAME)) {
        Serial.println("Warning: both highlight and unhighlight frame flags are set at the same time. This should not happen.");
    }
    
    if (is_flag_set(flags,GPS_DO_HIGHLIGHT_FRAME)) {
        highlight_compass_frame();
        set_flag(flags,GPS_CHANGED_COMPASS_GRID);
    }

    if (is_flag_set(flags,GPS_DO_UNHIGHLIGHT_FRAME)) {
        unhighlight_compass_frame();
        set_flag(flags,GPS_CHANGED_COMPASS_GRID);
    }

    if (is_flag_set(flags,GPS_CHANGED_COMPASS_GRID)) {

        display_refresh_compass();
    }

    clear_all_flags(flags);
}

uint8_t gps_update_needle_bearing(){
    double new_bearing = waypoint_bearing - magnetometer_bearing ; 
    if (new_bearing < 0) {
        new_bearing += 2 * M_PI;
    }
    if(fabs(new_bearing - needle_bearing) < 0.05){
        return 0;
    }
    needle_bearing = new_bearing;
    return 1;
}


//================BLINKING PART=======================

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