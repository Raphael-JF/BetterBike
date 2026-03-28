#include <TinyGPS++.h>
#include "gps_core.h"


bool is_gps_active = 0;
double bearing_to_display = 0;
double last_bearing_to_display = -10.0;

struct position waypoint_position = {44.80573206597555, -0.6048971440442654}; // école

struct position current_position = {0.0, 0.0};


void update_current_position() {
    if (gps.location.isValid()) {
        current_position.lat = gps.location.lat();
        current_position.lng = gps.location.lng();
        
    }
}



TinyGPSPlus gps;
HardwareSerial gpsSerial(1);