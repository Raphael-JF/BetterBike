#include "Arduino.h"
#include <TinyGPS++.h>

#include "gps_core.h"
#include "time/blinking.h"
#include "lcd/lcd_core.h"


TinyGPSPlus gps;
HardwareSerial gpsSerial(1);


struct position waypoint_position = {44.80573206597555, -0.6048971440442654}; // école
struct position current_position = {0.0, 0.0};
unsigned long last_gps_sync_millis = 0;


enum gps_timeout_status timeout_status = GPS_TIMEOUT_STATUS_INVALID;




void update_current_position() {
    current_position.lat = gps.location.lat();
    current_position.lng = gps.location.lng();
    last_gps_sync_millis = millis();
}


uint8_t update_gps_timeout_status() {
    unsigned long current_millis = millis();
    switch (timeout_status) {
        case GPS_TIMEOUT_STATUS_OK:
            if (current_millis - last_gps_sync_millis > GPS_TIMEOUT_OLD){
                // "OK" -> "OLD", faire clignoter le cadre de la boussole pour indiquer que les données GPS sont anciennes
                timeout_status = GPS_TIMEOUT_STATUS_OLD;
                blinking_start(IDX_COMPASS_FRAME_BLINKING, &compass_frame_blinking);
                update_compass_frame_blinking(); 
                return 1;
            }
            break;
        case GPS_TIMEOUT_STATUS_OLD:
            // faire clignoter le cadre de la boussole pour indiquer que les données GPS sont anciennes
            if (current_millis - last_gps_sync_millis < GPS_TIMEOUT_OLD) {
                // "OLD" -> "OK", arrêter de faire clignoter le cadre de la boussole
                timeout_status = GPS_TIMEOUT_STATUS_OK;
                blinking_stop(IDX_COMPASS_FRAME_BLINKING);
                unhighlight_compass_frame();  // enlever le contour de la boussole
                return 1;
            }
            if (current_millis - last_gps_sync_millis > GPS_TIMEOUT_INVALID) {
                // "OLD" -> "INVALID", activer le contour de la boussole pour indiquer que les données GPS sont invalides
                timeout_status = GPS_TIMEOUT_STATUS_INVALID;
                blinking_stop(IDX_COMPASS_FRAME_BLINKING);
                highlight_compass_frame(); // ajouter le contour de la boussole pour indiquer que les données GPS sont invalides
                return 1;
            }
            break;
        case GPS_TIMEOUT_STATUS_INVALID:
            if (current_millis - last_gps_sync_millis < GPS_TIMEOUT_OLD) {
                // "INVALID" -> "OK"
                timeout_status = GPS_TIMEOUT_STATUS_OK;
                unhighlight_compass_frame();
                return 1;
            }
            break;
    }
    return 0;
}