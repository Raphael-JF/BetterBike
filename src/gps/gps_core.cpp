#include "Arduino.h"
#include <TinyGPS++.h>

#include "gps_core.h"
#include "time/blinking.h"


TinyGPSPlus gps;
HardwareSerial gpsSerial(1);


struct position waypoint_position = {44.80573206597555, -0.6048971440442654}; // école
struct position current_position = {0.0, 0.0};
unsigned long last_gps_sync_millis = 0;


enum gps_timeout_status timeout_status = GPS_TIMEOUT_STATUS_OK;




void update_current_position() {
    current_position.lat = gps.location.lat();
    current_position.lng = gps.location.lng();
    last_gps_sync_millis = millis();
}



