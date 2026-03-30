#ifndef GPS_CORE_H
#define GPS_CORE_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>


struct position {
    double lat;
    double lng;
};

extern  TinyGPSPlus gps;
extern HardwareSerial gpsSerial;


extern struct position waypoint_position;
extern struct position current_position;

extern unsigned long last_gps_sync_millis;

enum gps_timeout_status {
    GPS_TIMEOUT_STATUS_OK,
    GPS_TIMEOUT_STATUS_OLD,
    GPS_TIMEOUT_STATUS_INVALID
};
extern enum gps_timeout_status timeout_status;

#define GPS_TIMEOUT_OLD 5000 // en ms, durée après laquelle on considère que les données GPS sont vieilles (mais pas encore invalides) si pas de nouvelle donnée reçue
#define GPS_TIMEOUT_INVALID 10000 // en ms, durée après laquelle on considère que les données GPS sont invalides si pas de nouvelle donnée reçue


void update_current_position();


#endif // GPS_CORE_H