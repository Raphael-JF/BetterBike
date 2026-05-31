#include "magnetometer/magnetometer.h"

#include <Arduino.h>
#include <math.h>

#if defined(ESP32)
#include <Preferences.h>
#endif

double magnetometer_bearing = 0.0;
struct magnetometer_compensator magnetometer_compensator;

static uint8_t magnetometer_stream_raw_enabled = 0;
static uint16_t magnetometer_stream_raw_period_ms = 20; // 50 Hz by default
static unsigned long magnetometer_stream_raw_last_ms = 0;

static void magnetometer_load_compensator_from_flash() {
#if defined(ESP32)
    Preferences prefs;

    // Namespace "mag" stores magnetometer calibration data.
    // Keys:
    //  - "x_off": int16 offset for X
    //  - "y_off": int16 offset for Y
    //
    // If not present, defaults to 0.
    if (!prefs.begin("mag", true)) {
        magnetometer_compensator.x_offset = 0;
        magnetometer_compensator.y_offset = 0;
        Serial.println("Magnetometer: Preferences begin failed, using offsets 0/0");
        return;
    }

    // Preferences stores integers as 32-bit; we cast down to int16_t.
    int32_t x = prefs.getInt("x_off", 0);
    int32_t y = prefs.getInt("y_off", 0);
    prefs.end();

    magnetometer_compensator.x_offset = (int16_t)x;
    magnetometer_compensator.y_offset = (int16_t)y;

    Serial.print("Magnetometer: loaded offsets from flash: x_offset=");
    Serial.print((int)magnetometer_compensator.x_offset);
    Serial.print(" y_offset=");
    Serial.println((int)magnetometer_compensator.y_offset);
#else
    // Non-ESP32 targets: no flash preferences storage implemented here.
    magnetometer_compensator.x_offset = 0;
    magnetometer_compensator.y_offset = 0;
#endif
}

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

    // Load previously-saved calibration offsets from ESP32 flash (NVS).
    magnetometer_load_compensator_from_flash();

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
}
