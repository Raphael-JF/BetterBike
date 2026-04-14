#include "magnetometer.h"


double magnetometer_bearing = 0.0;
struct magnetometer_compensator magnetometer_compensator;


bool init_magnetometer(){
    if (!qmc5883p_begin(&Wire, QMC5883P_SLAVE_ADDRESS)) {
        return false;
    }
    if (qmc5883p_configure()) {
        Serial.println("Magnetometer configured successfully.");
    } else {
        Serial.println("Magnetometer configuration failed.");
        while (1);
    }
    return true;
}


uint8_t read_magnetometer_data(){
    return qmc5883p_read_raw();
}

uint8_t update_magnetometer_bearing() {
    double new_bearing = qmc5883p_compute_heading_radians();

    if (fabs(magnetometer_bearing - new_bearing) < 0.05) {
        return 0;
    }
    magnetometer_bearing = new_bearing;
    // Serial.print("Time: "); Serial.print(millis()/1000U, DEC);
    // Serial.print("| X:");
    // Serial.print(raw_data.x, DEC);
    // Serial.print("| Y:");
    // Serial.println(raw_data.y, DEC);
    return 1;
}

