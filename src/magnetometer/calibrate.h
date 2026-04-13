/**
 * @file calibrate.h
 * @brief Magnetometer calibration module
 * 
 * This module manages the calibration of the QMC5883P magnetometer.
 * It provides functions to acquire min/max values on X and Y axes,
 * compute compensation offsets, and display raw sensor data.
 */

#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <stdint.h>
#include "magnetometer/magnetometer.h"

/**
 * @brief Acquire minimum and maximum values for X and Y axes
 * 
 * This function should be called periodically to update the detected
 * minimum and maximum values. Must be called multiple times while
 * performing a complete rotation of the sensor to obtain accurate
 * calibration data.
 */
void magnetometer_calibrate_aquire();

/**
 * @brief Display current calibration values
 * 
 * Prints the min and max values for X and Y axes to the serial port.
 * Format: "x_min: XXX | x_max: XXX | y_min: XXX | y_max: XXX"
 */
void magnetometer_calibrate_print();

/**
 * @brief Reset calibration values
 * 
 * Resets all min/max values to extreme values
 * (INT16_MIN and INT16_MAX) to restart acquisition.
 */
void magnetometer_calibrate_reset();

/**
 * @brief Compute and apply compensation offsets
 * 
 * Uses the acquired min/max values to calculate compensation offsets
 * (axis centers) and applies them to the magnetometer_compensator structure.
 */
void magnetometer_calibrate_compute_offsets();



#endif // CALIBRATE_H