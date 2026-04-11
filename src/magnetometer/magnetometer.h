#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <Arduino.h>
#include <Wire.h>
#include "librairies/SensorLib/SensorQMC5883P.hpp"

extern SensorQMC5883P magnetometer;
extern MagnetometerData magnetometer_data;
extern float magnetometer_bearing;


/*
    Initialise le magnétomètre avec les paramètres de configuration souhaités et la déclinaison magnétique locale.
*/
void init_magnetometer();


/*
    Lit les données du magnétomètre et met à jour magnetometer_bearing. Retourne 1 si la lecture a réussi et que magnetometer_bearing a été mis à jour, 0 sinon.
*/
uint8_t update_magnetometer_bearing();


/*
    Met à jour bearing_to_display en radians pour pointer du current_position vers waypoint_position, en utilisant magnetometer_bearing pour calculer l'angle de l'aiguille de la boussole. Normalise l'angle entre 0 et 2*PI. Retourne 1 si bearing_to_display a été changé, 0 sinon
*/
uint8_t update_bearing_to_display();


#endif // MAGNETOMETER_H