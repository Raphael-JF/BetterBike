#include "qmc5883p_raw.h"

#include <math.h>


//Find the magnetic declination : https://www.magnetic-declination.com/
const double declination_rad = radians(1.0 + (20.0 / 60.0));  // Bordeaux
TwoWire *qmc5883p_wire;



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

bool qmc5883p_begin( TwoWire *wire, uint8_t address)
{
    

    if (wire == nullptr) {
        return false;
    }

    qmc5883p_wire = wire;

    const int status = read_register(REG_STATUS);
    return status >= 0;
}

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

double qmc5883p_compute_heading_degrees()
{
    return qmc5883p_compute_heading_radians() * 180.0 / M_PI;
}