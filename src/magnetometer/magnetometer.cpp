#include "magnetometer.h"
#include "gps/gps_core.h"
#include "display/compass.h"

SensorQMC5883P magnetometer;
MagnetometerData magnetometer_data;
float magnetometer_bearing = 0.0f;




void init_magnetometer(){
    

    // The desired output data rate in Hz.  Allowed values are 10.0, 50.0, 100.0 and 200.0HZ.
    float data_rate_hz = 100.0f;
    // op_mode: Allowed values are SUSPEND, NORMAL, SINGLE_MEASUREMENT, CONTINUOUS_MEASUREMENT
    OperationMode op_mode = OperationMode::NORMAL;
    // full_scale: Allowed values are FS_2G, FS_8G, FS_12G, FS_30G
    MagFullScaleRange full_scale = MagFullScaleRange::FS_8G;
    // over_sample_ratio: Allowed values are OSR_1, OSR_2, OSR_4, OSR_8
    MagOverSampleRatio over_sample_ratio = MagOverSampleRatio::OSR_4;
    // down_sample_ratio: Allowed values are DSR_1, DSR_2, DSR_4, DSR_8
    MagDownSampleRatio down_sample_ratio = MagDownSampleRatio::DSR_1;

    /* Config Magnetometer */
    if (magnetometer.configMagnetometer(
                op_mode,
                full_scale,
                data_rate_hz,
                over_sample_ratio,
                down_sample_ratio)) {
        Serial.println("Magnetometer configured successfully.");
    } else {
        Serial.println("Magnetometer configuration failed.");
        while (1);
    }

    //Find the magnetic declination : https://www.magnetic-declination.com/
    float declination_deg = MagnetometerUtils::dmsToDecimalDegrees(1, 20);  // in France, Bordeaux

    magnetometer.setDeclination(declination_deg);
}


uint8_t update_magnetometer_bearing() {
    if(!magnetometer.readData(magnetometer_data)){
        return 0;
    }
    if (fabs(magnetometer_bearing - magnetometer_data.heading) < 0.0001f) {
        return 0;
    }
    // Serial.print("Time: "); Serial.print(millis());
    // Serial.print("| X:");
    // // Affichage des données brutes du magnétomètre pour le debug (uint16_t pour éviter les problèmes d'affichage de int16_t négatifs)
    // Serial.print(magnetometer_data.raw.x, DEC);
    // Serial.print("| Y:");
    // Serial.println(magnetometer_data.raw.y, DEC);
    Serial.print("|bearing: ");
    Serial.println(360 - magnetometer_data.heading_degrees, 2);
    magnetometer_bearing = 2*M_PI - magnetometer_data.heading;
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