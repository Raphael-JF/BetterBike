#include "utils/component.h"

void warn_component(struct component c, uint8_t flag){  
    set_flag(c.flags, flag);
}