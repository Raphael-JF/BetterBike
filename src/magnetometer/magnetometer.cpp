#include "magnetometer/magnetometer.h"

#include <Arduino.h>
#include <math.h>

double magnetometer_bearing = 0.0;
struct magnetometer_compensator magnetometer_compensator;

static uint8_t magnetometer_stream_raw_enabled = 0;
static uint16_t magnetometer_stream_raw_period_ms = 20; // 50 Hz by default
static unsigned long magnetometer_stream_raw_last_ms = 0;

bool init_magnetometer() {
    if (!qmc5883p_begin(&Wire, QMC5883P_SLAVE_ADDRESS)) {
        return false;
    }
    if (qmc5883p_configure()) {
        Serial.println("Magnetometer configured successfully.");
    } else {
        Serial.println("Magnetometer configuration failed.");
        while (1) {
        }
    }

    magnetometer_compensator.x_offset = 0;
    magnetometer_compensator.y_offset = 0;

    return true;
}

uint8_t read_magnetometer_data() {
    return qmc5883p_read_raw();
}

uint8_t update_magnetometer_bearing() {
    double new_bearing = qmc5883p_compute_heading_radians();

    if (fabs(magnetometer_bearing - new_bearing) < 0.05) {
        return 0;
    }
    magnetometer_bearing = new_bearing;
    return 1;
}

void magnetometer_stream_raw_enable(uint8_t enable) {
    magnetometer_stream_raw_enabled = enable ? 1 : 0;
    magnetometer_stream_raw_last_ms = 0;
}

void magnetometer_stream_raw_set_period_ms(uint16_t period_ms) {
    if (period_ms == 0) {
        period_ms = 1;
    }
    magnetometer_stream_raw_period_ms = period_ms;
}

void magnetometer_stream_raw_tick(void) {
    if (!magnetometer_stream_raw_enabled) {
        return;
    }

    const unsigned long now = millis();
    if (magnetometer_stream_raw_last_ms != 0 &&
        (unsigned long)(now - magnetometer_stream_raw_last_ms) < (unsigned long)magnetometer_stream_raw_period_ms) {
        return;
    }
    magnetometer_stream_raw_last_ms = now;

    // Only print when we have new data.
    if (!read_magnetometer_data()) {
        return;
    }

    // Output format: MAG_RAW,<ms>,<x>,<y>,<z>
    Serial.print("MAG_RAW,");
    Serial.print(now);
    Serial.print(",");
    Serial.print((int)raw_data.x);
    Serial.print(",");
    Serial.print((int)raw_data.y);
    Serial.print(",");
    Serial.println((int)raw_data.z);
}
