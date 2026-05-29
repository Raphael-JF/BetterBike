#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <Arduino.h>
#include <Wire.h>
#include "qmc5883p_raw.h"


#include "gps/gps_core.h"




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
    Lit les données brutes du magnétomètre et les stocke dans raw_data. Retourne 1 si la lecture a réussi et que raw_data a été mis à jour, 0 sinon.
*/
uint8_t read_magnetometer_data();


/*
    Lit les données du magnétomètre et met à jour magnetometer_bearing. Retourne 1 si la lecture a réussi et que magnetometer_bearing a été mis à jour, 0 sinon.
*/
uint8_t update_magnetometer_bearing();

/*
*  Enables or disables streaming of raw magnetometer data to the serial port. When enabled, the raw X and Y values will be printed as CSV lines at the interval defined by magnetometer_stream_raw_set_period_ms().
*/
void magnetometer_stream_raw_enable(uint8_t enable);

/*
*  Sets the period for streaming raw magnetometer data to the serial port, in milliseconds.
*/
void magnetometer_stream_raw_set_period_ms(uint16_t period_ms);

/*
* Should be called regularly (e.g. in the main loop) to handle streaming of raw magnetometer data if enabled. It will print a new line with raw X and Y values in CSV format at the defined interval.
*/
void magnetometer_stream_raw_tick(void);
#endif // MAGNETOMETER_H
