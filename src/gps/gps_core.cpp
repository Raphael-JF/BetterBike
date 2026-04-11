#include "Arduino.h"
#include <TinyGPS++.h>
#include <math.h>

#include "gps_core.h"
#include "time/blinking.h"
#include "lcd/lcd_core.h"


TinyGPSPlus gps;
HardwareSerial gpsSerial(1);


struct position waypoint_position = {48.877234, 2.320267}; // école
struct position current_position = {0.0, 0.0};
double waypoint_bearing = 0.0; // en radians, angle entre le nord et la ligne current_position -> waypoint_position, dans le sens des aiguilles d'une montre
unsigned long last_gps_sync_millis = 0;


enum gps_timeout_status timeout_status = GPS_TIMEOUT_STATUS_INVALID;




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


uint8_t update_gps_timeout_status() {
    unsigned long current_millis = millis();
    switch (timeout_status) {
        case GPS_TIMEOUT_STATUS_OK:
            if (current_millis - last_gps_sync_millis > GPS_TIMEOUT_OLD){
                // "OK" -> "OLD", faire clignoter le cadre de la boussole pour indiquer que les données GPS sont anciennes
                timeout_status = GPS_TIMEOUT_STATUS_OLD;
                blinking_start(IDX_COMPASS_FRAME_BLINKING, &compass_frame_blinking);
                return 1;
            }
            break;
        case GPS_TIMEOUT_STATUS_OLD:
            if (current_millis - last_gps_sync_millis < GPS_TIMEOUT_OLD) {
                // "OLD" -> "OK", arrêter de faire clignoter le cadre de la boussole
                timeout_status = GPS_TIMEOUT_STATUS_OK;
                blinking_stop(IDX_COMPASS_FRAME_BLINKING);
                highlight_compass_frame(); 
                return 1;
            }
            if (current_millis - last_gps_sync_millis > GPS_TIMEOUT_INVALID) {
                // "OLD" -> "INVALID", Désactiver le contour de la boussole pour indiquer que les données GPS sont invalides
                timeout_status = GPS_TIMEOUT_STATUS_INVALID;
                blinking_stop(IDX_COMPASS_FRAME_BLINKING);
                unhighlight_compass_frame(); // enlever le contour de la boussole pour indiquer que les données GPS sont invalides
                return 1;
            }
            break;
        case GPS_TIMEOUT_STATUS_INVALID:
            if (current_millis - last_gps_sync_millis < GPS_TIMEOUT_OLD) {
                // "INVALID" -> "OK"
                timeout_status = GPS_TIMEOUT_STATUS_OK;
                highlight_compass_frame();
                return 1;
            }
            break;
    }
    return 0;
}