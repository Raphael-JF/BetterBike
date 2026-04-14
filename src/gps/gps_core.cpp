#include "Arduino.h"
#include <TinyGPS++.h>
#include <math.h>


#include "gps_core.h"


TinyGPSPlus gps;
HardwareSerial gpsSerial(1);


struct position waypoint_position = {48.877234, 2.320267}; // école
struct position current_position = {0.0, 0.0};
double waypoint_bearing = 0.0; // en radians, angle entre le nord et la ligne current_position -> waypoint_position, dans le sens des aiguilles d'une montre
unsigned long last_gps_sync_millis = 0;
enum gps_timeout_status timeout_status = GPS_TIMEOUT_STATUS_INVALID;
flag_manager* gps_data_flags = create_flag_manager(NUM_GPS_FLAGS);





void update_current_position() {
    current_position.lat = gps.location.lat();
    current_position.lng = gps.location.lng();
    last_gps_sync_millis = millis();
}


uint8_t update_waypoint_bearing() {
    double bearing_degrees = gps.courseTo(
        current_position.lat,
        current_position.lng,
        waypoint_position.lat,
        waypoint_position.lng
    );

    if (!isfinite(bearing_degrees)) {
        return 0;
    }

    double new_bearing = radians(bearing_degrees);

    if (fabs(new_bearing - waypoint_bearing) < 0.0001) {
        return 0;
    }

    waypoint_bearing = new_bearing;
    return 1;
}


uint8_t read_gps_data() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    if(gps.time.isValid() && gps.time.isUpdated() && gps.time.age() < 2000){
        set_flag(gps_data_flags, GPS_TIME_FRESH);
    }
    if(gps.date.isValid() && gps.date.isUpdated() && gps.date.age() < 2000){
        set_flag(gps_data_flags, GPS_DATE_FRESH);
    }
    if(gps.location.isValid() && gps.location.isUpdated() && gps.location.age() < 2000){
        set_flag(gps_data_flags, GPS_FIX_FRESH);
    }


    if (is_flag_set(gps_data_flags, GPS_DATE_FRESH)) {
        utc_day = gps.date.day();
        utc_month = gps.date.month();
        utc_year = gps.date.year();
    }


    if (is_flag_set(gps_data_flags, GPS_FIX_FRESH)) {
        update_current_position();
        if (is_flag_set(gps_data_flags, GPS_TIME_FRESH)) {
            last_got_gps_time.hours = gps.time.hour();
            last_got_gps_time.minutes = gps.time.minute();
        }
        clear_all_flags(gps_data_flags);
        return 1;

    }
    clear_all_flags(gps_data_flags);
    return 0;
}