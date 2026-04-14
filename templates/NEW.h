#ifndef NEW_H
#define NEW_H


#include "utils/flag_manager.h"
#include "utils/component.h"




enum NEW_flags : uint8_t {
    NEW_FLAG_1 = 0,
    NUM_NEW_FLAGS = 1,
};


extern struct component NEW;

void NEW_component_on_enter();
void NEW_update();


#endif // NEW_H
