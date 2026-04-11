#include "magnetometer.h"

// #include "librairies/SensorLib/SensorQMC5883P.hpp"
SensorQMC5883P magnetometer;


void init_magnetometer(){
    

    // The desired output data rate in Hz.  Allowed values are 10.0, 50.0, 100.0 and 200.0HZ.
    float data_rate_hz = 10.0f;
    // op_mode: Allowed values are SUSPEND, NORMAL, SINGLE_MEASUREMENT, CONTINUOUS_MEASUREMENT
    OperationMode op_mode = OperationMode::NORMAL;
    // full_scale: Allowed values are FS_2G, FS_8G, FS_12G, FS_30G
    MagFullScaleRange full_scale = MagFullScaleRange::FS_2G;
    // over_sample_ratio: Allowed values are OSR_1, OSR_2, OSR_4, OSR_8
    MagOverSampleRatio over_sample_ratio = MagOverSampleRatio::OSR_1;
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