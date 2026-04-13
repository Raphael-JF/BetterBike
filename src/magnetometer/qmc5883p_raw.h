#ifndef QMC5883P_RAW_H
#define QMC5883P_RAW_H

#include <Arduino.h>
#include <Wire.h>

#define QMC5883P_SLAVE_ADDRESS 0x2C

#define REG_DATA_X_L (0x01)
#define REG_STATUS (0x09)
#define REG_CMD1 (0x0A)
#define REG_CMD2 (0x0B)





//Find the magnetic declination : https://www.magnetic-declination.com/
extern const double declination_rad;

extern struct magnetometer_data raw_data;


bool qmc5883p_begin(TwoWire *wire, uint8_t address);
bool qmc5883p_configure();
bool qmc5883p_read_raw();
double qmc5883p_compute_heading_radians();
double qmc5883p_compute_heading_degrees();

#endif // QMC5883P_RAW_H