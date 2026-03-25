#ifndef GPS_CORE_H
#define GPS_CORE_H

#include <TinyGPS++.h>

#if defined(ARDUINO_ARCH_ESP32)
#include <HardwareSerial.h>
constexpr int GPS_RX_PIN = 16;
constexpr int GPS_TX_PIN = 17;
#else
#include <SoftwareSerial.h>
#endif

extern bool is_gps_active;

extern double bearing_to_display;
extern double last_bearing_to_display;

extern  TinyGPSPlus gps;
#if defined(ARDUINO_ARCH_ESP32)
extern HardwareSerial gpsSerial;
#else
extern SoftwareSerial gpsSerial; // RX, TX
#endif

#endif // GPS_CORE_H