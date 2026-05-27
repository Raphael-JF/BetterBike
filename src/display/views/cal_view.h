#ifndef CAL_VIEW_H
#define CAL_VIEW_H

#include <stdlib.h>
#include <math.h>


#include "display/components/compass.h"
#include "display/components/texts.h"
#include "display/lcd_core.h"

#include "utils/flag_manager.h"
#include "utils/component.h"

#include "magnetometer/calibrate.h"


enum cal_view_flags : uint8_t {
    CAL_CHANGED_MAGNETOMETER_RAW_DATA = 0,
    CAL_CHANGED_MAGNETOMETER_BOUNDARIES = 1,
    NUM_CAL_VIEW_FLAGS = 2,
};

extern struct flag_manager* cal_view_flags;


void enter_cal_view();
void update_cal_view();




#endif // CAL_VIEW_H
