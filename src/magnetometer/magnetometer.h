#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <Arduino.h>
#include <Wire.h>
#include "qmc5883p_raw.h"





struct magnetometer_compensator {
    int16_t x_offset;
    int16_t y_offset;
    // double scale;
};

extern double magnetometer_bearing;
extern struct magnetometer_compensator magnetometer_compensator;


/*
    Initialise le magnétomètre avec les paramètres de configuration souhaités et la déclinaison magnétique locale.
*/
bool init_magnetometer();


/*
    Lit les données du magnétomètre et met à jour magnetometer_bearing. Retourne 1 si la lecture a réussi et que magnetometer_bearing a été mis à jour, 0 sinon.
*/
uint8_t update_magnetometer_bearing();


/*
    Met à jour bearing_to_display en radians pour pointer du current_position vers waypoint_position, en utilisant magnetometer_bearing pour calculer l'angle de l'aiguille de la boussole. Normalise l'angle entre 0 et 2*PI. Retourne 1 si bearing_to_display a été changé, 0 sinon
*/
uint8_t update_bearing_to_display();


#endif // MAGNETOMETER_H