#ifndef CAL_COMPASS_H
#define CAL_COMPASS_H

#include "display/compass/compass_core.h"

#include "utils/flag_manager.h"
#include "utils/component.h"



enum cal_compass_flags : uint8_t {
    
    NUM_CAL_COMPASS_FLAGS = 0
};


extern struct component Cal_compass;

void cal_compass_component_on_enter();
void cal_compass_component_update();


#endif // CAL_COMPASS_H
