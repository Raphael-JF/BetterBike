/**
 * @file qmc5883p_raw.cpp
 * @brief QMC5883P magnetometer driver implementation
 */

#include "qmc5883p_raw.h"

#include <math.h>

/**
 * @brief Magnetic declination correction in radians for Bordeaux, France.
 * @details Approximately 1° 20' = 1.348° ≈ 0.0235 radians
 * @see https://www.magnetic-declination.com/
 */
//Find the magnetic declination : https://www.magnetic-declination.com/
const double declination_rad = radians(1.0 + (20.0 / 60.0));  // Bordeaux

/** @brief Pointer to the TwoWire I2C interface object */
TwoWire *qmc5883p_wire;

/** @brief Raw magnetometer sensor readings (X, Y, Z axes) */
struct magnetometer_data raw_data;


/**
 * @brief Write a single register value to the QMC5883P sensor.
 * @param[in] reg Register address to write to
 * @param[in] value 8-bit value to write
 * @return true if write succeeded, false if I2C error or uninitialized
 * @note For internal use only. Requires qmc5883p_begin() to be called first.
 */
bool write_register( uint8_t reg, uint8_t value)
{
    if (qmc5883p_wire == nullptr) {
        return false;
    }

    qmc5883p_wire->beginTransmission(QMC5883P_SLAVE_ADDRESS);
    qmc5883p_wire->write(reg);
    qmc5883p_wire->write(value);
    return qmc5883p_wire->endTransmission() == 0;
}

/**
 * @brief Read a single register value from the QMC5883P sensor.
 * @param[in] reg Register address to read from
 * @return Register value (0-255) on success, -1 on I2C error or uninitialized
 * @note For internal use only. Requires qmc5883p_begin() to be called first.
 */
int read_register(uint8_t reg)
{
    if (qmc5883p_wire == nullptr) {
        return -1;
    }

    qmc5883p_wire->beginTransmission(QMC5883P_SLAVE_ADDRESS);
    qmc5883p_wire->write(reg);
    if (qmc5883p_wire->endTransmission(false) != 0) {
        return -1;
    }

    size_t count = qmc5883p_wire->requestFrom((int)QMC5883P_SLAVE_ADDRESS, 1);
    if (count != 1) {
        return -1;
    }
    return qmc5883p_wire->read();
}

/**
 * @brief Read multiple register values from the QMC5883P sensor.
 * @param[in] reg Starting register address
 * @param[out] buffer Pointer to buffer to store read values
 * @param[in] len Number of bytes to read
 * @return true if read succeeded, false on I2C error or uninitialized
 * @note For internal use only. Requires qmc5883p_begin() to be called first.
 */
bool read_registers( uint8_t reg, uint8_t *buffer, size_t len)
{
    if (qmc5883p_wire == nullptr || buffer == nullptr || len == 0) {
        return false;
    }

    qmc5883p_wire->beginTransmission(QMC5883P_SLAVE_ADDRESS);
    qmc5883p_wire->write(reg);
    if (qmc5883p_wire->endTransmission(false) != 0) {
        return false;
    }

    // size_t count = qmc5883p_wire->requestFrom((int)QMC5883P_SLAVE_ADDRESS, (int)len);
    size_t count = qmc5883p_wire->requestFrom(QMC5883P_SLAVE_ADDRESS, len);
    
    if (count != len) {
        return false;
    }

    for (size_t i = 0; i < len; ++i) {
        if (!qmc5883p_wire->available()) {
            return false;
        }
        buffer[i] = (uint8_t)qmc5883p_wire->read();
    }
    return true;

}

/**
 * @brief Initialize I2C connection and verify sensor presence.
 * @param[in] wire Pointer to TwoWire I2C interface object
 * @param[in] address Target I2C slave address (default: QMC5883P_SLAVE_ADDRESS)
 * @return true if sensor responds to queries, false on error
 * @note Must be called before any other sensor operations
 */
bool qmc5883p_begin( TwoWire *wire, uint8_t address)
{
    

    if (wire == nullptr) {
        return false;
    }

    qmc5883p_wire = wire;

    const int status = read_register(REG_STATUS);
    return status >= 0;
}

/**
 * @brief Configure sensor for continuous measurement operation.
 * @details Applied configuration:
 *   - Mode: NORMAL (continuous measurement enabled)
 *   - Oversampling Ratio (OSR): 4x (noise reduction)
 *   - Data Output Rate (ODR): 100 Hz
 *   - Range: ±8 Gauss full scale
 *   - Downsampling Ratio (DSR): 1x (no downsampling)
 * @return true if configuration succeeded, false on I2C error
 * @note Should be called once after qmc5883p_begin()
 */
bool qmc5883p_configure()
{
    if (!write_register(REG_CMD2, 0x80)) {
        return false;
    }
    delay(10);
    if (!write_register(REG_CMD2, 0x00)) {
        return false;
    }

    const uint8_t osr_4 = (0x01u << 4);
    const uint8_t dsr_1 = (0x00u << 6);
    const uint8_t odr_100hz = (0x02u << 2);
    const uint8_t mode_normal = 0x01u;
    const uint8_t cmd1 = dsr_1 | osr_4 | odr_100hz | mode_normal;

    return write_register(REG_CMD1, cmd1);
}

/**
 * @brief Read raw sensor data from QMC5883P.
 * @details Checks data-ready flag, reads all 6 bytes (X, Y, Z LSB/MSB), 
 * and converts to int16_t values. Updates global raw_data structure.
 * Overflow flag is checked but not stored (data remains valid on overflow).
 * @return true if data was successfully read and is ready, false if not ready or I2C error
 * @note Should be called periodically (≤10ms intervals at 100Hz ODR)
 */
bool qmc5883p_read_raw()
{

    const int status = read_register(REG_STATUS);
    if (status < 0) {
        return false;
    }

    uint8_t overflow = ((status & (1u << 1)) != 0u);
    if ((status & 0x01u) == 0u) {
        return false;
    }

    uint8_t buffer[6] = {0};
    if (!read_registers(REG_DATA_X_L, buffer, sizeof(buffer))) {
        return false;
    }

    raw_data.x = (int16_t)(((uint16_t)buffer[1] << 8) | buffer[0]);
    raw_data.y = (int16_t)(((uint16_t)buffer[3] << 8) | buffer[2]);
    raw_data.z = (int16_t)(((uint16_t)buffer[5] << 8) | buffer[4]);

    return true;
}

/**
 * @brief Compute magnetic heading from raw data in radians.
 * @details Calculates bearing angle using arctangent of Y/X ratio,
 * applies declination correction, and normalizes result to [0, 2π).
 * @return Heading in radians: 0=North, π/2=East, π=South, 3π/2=West
 * @note Requires raw_data with calibration offsets already applied
 * @see qmc5883p_compute_heading_degrees() for degree output
 */
double qmc5883p_compute_heading_radians()
{
    double heading = atan2((double)raw_data.y, (double)raw_data.x);
    heading += declination_rad;

    heading = fmod(heading, 2.0 * M_PI);
    if (heading < 0.0) {
        heading += 2.0 * M_PI;
    }
    return heading;
}

/**
 * @brief Compute magnetic heading from raw data in degrees.
 * @details Calls qmc5883p_compute_heading_radians() and converts result 
 * from radians to degrees (rad * 180 / π).
 * @return Heading in degrees (0-360): 0=North, 90=East, 180=South, 270=West
 * @note Requires raw_data with calibration offsets already applied
 * @see qmc5883p_compute_heading_radians() for radian output
 */
double qmc5883p_compute_heading_degrees()
{
    return qmc5883p_compute_heading_radians() * 180.0 / M_PI;
}