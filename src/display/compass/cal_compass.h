#ifndef CAL_COMPASS_H
#define CAL_COMPASS_H

#include "stdlib.h"
#include "math.h"


#include "display/compass/compass_core.h"

#include "utils/flag_manager.h"
#include "utils/component.h"

#include "magnetometer/calibrate.h"


#define NUM_PIXELS_CALIBRATION_CIRCLE 32 // multiples of 4 are great
#define NUM_COMPASS_PORTIONS 6
#define NUM_CALIBRATION_POINTS_PER_PORTION 1

enum cal_compass_flags : uint8_t {
    CAL_CHANGED_MAGNETOMETER_RAW_DATA = 0,
    CAL_CHANGED_MAGNETOMETER_BEARING = 1,
    CAL_CHANGED_BEARING_TO_DISPLAY = 2,
    CAL_CHANGED_PORTIONS = 3,
    CAL_CHANGED_NEEDLE_POSITION = 4,
    CAL_CHANGED_COMPASS_GRID = 5,
    NUM_CAL_COMPASS_FLAGS = 6,
};


extern struct component Cal_compass;

void cal_compass_on_enter();
void cal_compass_update();

void compass_grid_draw_portions();
uint8_t cal_update_bearing_to_display();


#endif // CAL_COMPASS_H
