#ifndef CAL_VIEW_H
#define CAL_VIEW_H

#include <stdlib.h>
#include <math.h>


#include "display/components/compass.h"
#include "display/lcd_core.h"

#include "utils/flag_manager.h"
#include "utils/component.h"

#include "magnetometer/calibrate.h"


#define NUM_PIXELS_CALIBRATION_CIRCLE 32 // multiples of 4 are great
#define NUM_COMPASS_PORTIONS 6
#define NUM_CALIBRATION_POINTS_PER_PORTION 1

enum cal_compass_flags : uint8_t {
    CAL_CHANGED_MAGNETOMETER_RAW_DATA = 0,
    CAL_CHANGED_MAGNETOMETER_BEARING = 1,
    CAL_CHANGED_needle_bearing = 2,
    CAL_CHANGED_PORTIONS = 3,
    CAL_CHANGED_NEEDLE_POSITION = 4,
    CAL_CHANGED_COMPASS_GRID = 5,
    NUM_CAL_COMPASS_FLAGS = 6,
};

extern struct flag_manager* cal_compass_flags;
extern struct component Cal_compass;

void enter_cal_view();
void update_cal_view();


void cal_compass_update();

void compass_grid_draw_portions();
uint8_t cal_update_needle_bearing();
uint8_t bearing_to_portion_index();


#endif // CAL_VIEW_H
