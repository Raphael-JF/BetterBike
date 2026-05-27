/**
 * @file calibrate.cpp
 * @brief Magnetometer calibration module implementation
 */

#include "calibrate.h"
#include <cstdint>

/** @brief Minimum detected value on the X axis (raw) */
int16_t x_min;

/** @brief Maximum detected value on the X axis (raw) */
int16_t x_max;

/** @brief Minimum detected value on the Y axis (raw) */
int16_t y_min;

/** @brief Maximum detected value on the Y axis (raw) */
int16_t y_max;

uint8_t magnetometer_calibrate_aquire(){
    uint8_t flag = 0;
    if(raw_data.x < x_min){
        x_min = raw_data.x;
        flag = 1;
    }
    if(raw_data.x > x_max){
        x_max = raw_data.x;
        flag = 1;
    }
    if(raw_data.y < y_min){
        y_min = raw_data.y;
        flag = 1;
    }
    if(raw_data.y > y_max){
        y_max = raw_data.y;
        flag = 1;
    }
    return flag;
}

void magnetometer_calibrate_print(){
    Serial.print("x_min: "); Serial.print(x_min, DEC);
    Serial.print(" | x_max: "); Serial.print(x_max, DEC);
    Serial.print(" | y_min: "); Serial.print(y_min, DEC);
    Serial.print(" | y_max: "); Serial.println(y_max, DEC);
}

void magnetometer_calibrate_reset(){
    x_min = INT16_MAX;
    x_max = INT16_MIN;
    y_min = INT16_MAX;
    y_max = INT16_MIN;
}

void magnetometer_calibrate_compute_offsets(){
    magnetometer_compensator.x_offset = ((uint32_t)x_max + (uint32_t)x_min) / 2UL;
    magnetometer_compensator.y_offset = ((uint32_t)y_max + (uint32_t)y_min) / 2UL;
}
