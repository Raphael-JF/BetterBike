#include "magnetometer.h"
#include "gps/gps_core.h"
#include "display/compass.h"

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


uint8_t update_magnetometer_bearing() {
    if(!qmc5883p_read_raw()){
        return 0;
    }
    double new_bearing = qmc5883p_compute_heading_radians();

    if (fabs(magnetometer_bearing - new_bearing) < 0.0001) {
        return 0;
    }
    magnetometer_bearing = new_bearing;
    Serial.print("Time: "); Serial.print(millis()/1000U, DEC);
    Serial.print("| X:");
    Serial.print(raw_data.x, DEC);
    Serial.print("| Y:");
    Serial.println(raw_data.y, DEC);
    return 1;
}


uint8_t update_bearing_to_display(){
    double new_bearing = magnetometer_bearing ; 
    // Normaliser l'angle entre 0 et 2*PI
    new_bearing = fmod(new_bearing, 2 * M_PI);
    if (new_bearing < 0) {
        new_bearing += 2 * M_PI;
    }
    if(fabs(new_bearing - bearing_to_display) < 0.0001){
        return 0;
    }
    bearing_to_display = new_bearing;
    return 1;
}