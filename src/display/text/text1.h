#ifndef text1_H
#define text1_H


#include "utils/flag_manager.h"
#include "utils/component.h"
#include "display/lcd_core.h"




enum text1_flags : uint8_t {
    TEXT1_HAS_CHANGED_TEXT = 0,
    NUM_TEXT1_FLAGS = 1,
};


extern struct component Text1;

void text1_component_on_enter();
void text1_update();


#endif // text1_H
