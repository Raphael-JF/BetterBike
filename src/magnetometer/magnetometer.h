#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <Arduino.h>
#include <Wire.h>
#include "librairies/SensorLib/SensorQMC5883P.hpp"

extern SensorQMC5883P magnetometer;
void init_magnetometer();

#endif // MAGNETOMETER_H