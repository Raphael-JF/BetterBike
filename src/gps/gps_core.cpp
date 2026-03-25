#include <TinyGPS++.h>
#if !defined(ARDUINO_ARCH_ESP32)
#include <SoftwareSerial.h>
#endif


bool is_gps_active = 0;
double bearing_to_display = 0;
double last_bearing_to_display = -10.0;



TinyGPSPlus gps;
#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial gpsSerial(1);
#else
SoftwareSerial gpsSerial(3, 4); // RX, TX
#endif