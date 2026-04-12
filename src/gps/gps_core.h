#ifndef GPS_CORE_H
#define GPS_CORE_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>



#define GPS_TIMEOUT_OLD 5000 // en ms, durée après laquelle on considère que les données GPS sont vieilles (mais pas encore invalides) si pas de nouvelle donnée reçue
#define GPS_TIMEOUT_INVALID 10000 // en ms, durée après laquelle on considère que les données GPS sont invalides si pas de nouvelle donnée reçue

struct position {
    double lat;
    double lng;
};

extern  TinyGPSPlus gps;
extern HardwareSerial gpsSerial;


extern struct position waypoint_position;
extern struct position current_position;
extern double waypoint_bearing; // en radians, angle entre le nord et la ligne current_position -> waypoint_position, dans le sens des aiguilles d'une montre


extern unsigned long last_gps_sync_millis;

enum gps_timeout_status {
    GPS_TIMEOUT_STATUS_OK,
    GPS_TIMEOUT_STATUS_OLD,
    GPS_TIMEOUT_STATUS_INVALID
};
extern enum gps_timeout_status timeout_status;


/*
    Met à jour la position actuelle en utilisant les données GPS.
*/
void update_current_position();


/*
    Met à jour bearing_to_display en radians pour pointer du current_position vers waypoint_position.
    Renvoie 1 si la valeur du bearing a changé, 0 sinon.
*/
uint8_t update_waypoint_bearing();


/*
    Met à jour le statut de timeout_status en fonction du temps écoulé depuis la dernière synchronisation avec les données GPS. Retourne 1 si l'affichage de la boussole doit être mis à jour, 0 sinon.
*/
uint8_t update_gps_timeout_status();


#endif // GPS_CORE_H