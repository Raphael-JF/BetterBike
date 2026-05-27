#ifndef COMPONENT_H
#define COMPONENT_H

#include "utils/flag_manager.h"

// Forward declaration for text struct example
struct text;

struct component {
    void (*on_enter)(void* self);
    void (*update)(void* self);
    struct flag_manager* flags;
};

void warn_component(struct component* c, uint8_t flag);

// Example function signatures for usage
void base_component_on_enter(void* self);
void base_component_update(void* self);

void text_on_enter(void* self);
void text_update(void* self);

#endif // COMPONENT_H
