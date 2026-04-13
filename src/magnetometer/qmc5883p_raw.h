#ifndef QMC5883P_RAW_H
#define QMC5883P_RAW_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @defgroup QMC5883P_RAW QMC5883P Raw Magnetometer Driver
 * @brief Minimal C-style driver for QMC5883P compass sensor over I2C.
 * @{
 */

/** @brief I2C slave address of the QMC5883P sensor */
#define QMC5883P_SLAVE_ADDRESS 0x2C

/** @brief Register address for X-axis LSB data */
#define REG_DATA_X_L (0x01)
/** @brief Register address for status flags */
#define REG_STATUS (0x09)
/** @brief Register address for control command 1 */
#define REG_CMD1 (0x0A)
/** @brief Register address for control command 2 */
#define REG_CMD2 (0x0B)



struct magnetometer_data {
    int16_t x;
    int16_t y;
    int16_t z;
};


/**
 * @brief Magnetic declination correction in radians.
 * @note Adjust using qmc5883p_set_declination_degrees() before computing headings.
 * @see https://www.magnetic-declination.com/
 */
extern const double declination_rad;

/**
 * @brief Raw magnetometer readings from the sensor.
 * @details Contains X, Y, Z axis values in raw sensor units (LSB) and overflow flag.
 */
extern struct magnetometer_data raw_data;

/**
 * @brief Initialize I2C communication with the QMC5883P sensor.
 * @param[in] wire Pointer to TwoWire object for I2C communication.
 * @param[in] address I2C slave address of the device.
 * @return true if sensor responds, false otherwise.
 */
bool qmc5883p_begin(TwoWire *wire, uint8_t address);

/**
 * @brief Configure the QMC5883P sensor for normal operation.
 * @details Sets the sensor to:
 *   - Mode: NORMAL (continuous measurement)
 *   - Data Rate: 100 Hz
 *   - Full Scale: ±8 Gauss
 *   - Oversampling: 4x
 *   - Downsampling: 1x
 * @return true if configuration succeeded, false on I2C error.
 */
bool qmc5883p_configure(void);

/**
 * @brief Read raw magnetic field data from the sensor.
 * @details Reads and updates @ref raw_data with X, Y, Z axis values.
 * Applies offset calibration automatically. Checks data ready flag before reading.
 * @return true if data was successfully read and is ready, false if not ready or I2C error.
 */
bool qmc5883p_read_raw(void);


/**
 * @brief Compute magnetic heading in radians.
 * @details Calculates compass heading from raw X, Y data and applies declination correction.
 * Result is normalized to [0, 2π) range.
 * @return Heading angle in radians (0 = north, π/2 = east, π = south, 3π/2 = west).
 * @note Requires valid raw_data with calibration offsets already applied.
 */
double qmc5883p_compute_heading_radians(void);

/**
 * @brief Compute magnetic heading in degrees.
 * @details Calculates compass heading from raw X, Y data and applies declination correction.
 * Result is normalized to [0, 360) range.
 * @return Heading angle in degrees (0 = north, 90 = east, 180 = south, 270 = west).
 * @note Requires valid raw_data with calibration offsets already applied.
 */
double qmc5883p_compute_heading_degrees(void);

/** @} */

#endif // QMC5883P_RAW_H