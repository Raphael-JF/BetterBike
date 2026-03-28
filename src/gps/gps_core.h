#ifndef GPS_CORE_H
#define GPS_CORE_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>
constexpr int GPS_RX_PIN = 16;
constexpr int GPS_TX_PIN = 17;

extern bool is_gps_active;

extern double bearing_to_display;
extern double last_bearing_to_display;

extern  TinyGPSPlus gps;
extern HardwareSerial gpsSerial;

#endif // GPS_CORE_H