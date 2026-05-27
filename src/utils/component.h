#ifndef COMPONENT_H
#define COMPONENT_H

#include "utils/flag_manager.h"

struct component {
    void (*on_enter)(void* self);
    void (*update)(void* self);
    struct flag_manager* flags;
};

void warn_component(struct component* c, uint8_t flag);

#endif // COMPONENT_H
