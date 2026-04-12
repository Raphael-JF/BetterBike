#ifndef COMPONENT_H
#define COMPONENT_H

#include "utils/flag_manager.h"

struct component {
    void (*onEnter)(void);
    void (*update)(void);
    struct flag_manager* flags;
};

void warn_component(struct component c, uint8_t flag);

#endif // COMPONENT_H