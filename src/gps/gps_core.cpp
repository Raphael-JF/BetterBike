#include <TinyGPS++.h>


bool is_gps_active = 0;
double bearing_to_display = 0;
double last_bearing_to_display = -10.0;



TinyGPSPlus gps;
HardwareSerial gpsSerial(1);