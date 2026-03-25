#ifndef GPS_CORE_H
#define GPS_CORE_H

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

extern bool is_gps_active;

extern double bearing_to_display;
extern double last_bearing_to_display;

extern  TinyGPSPlus gps;
extern SoftwareSerial gpsSerial; // RX, TX

#endif // GPS_CORE_H