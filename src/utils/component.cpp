#include "utils/component.h"

void warn_component(struct component* c, uint8_t flag){  
    set_flag(c->flags, flag);
}

// Example usage for a base component
void base_component_on_enter(void* self) {
    struct component* comp = (struct component*)self;
    // ... your logic here, e.g. set a flag
    set_flag(comp->flags, 0);
}

void base_component_update(void* self) {
    struct component* comp = (struct component*)self;
    // ... your logic here
}

// Example usage for a text component
// Suppose you have a struct text that extends component
// struct text { struct component base; ... }

void text_on_enter(void* self) {
    // Cast to your text struct
    struct text* txt = (struct text*)self;
    // ... your logic here, e.g. set a flag on the base
    set_flag(txt->base.flags, 1);
}

void text_update(void* self) {
    struct text* txt = (struct text*)self;
    // ... your logic here
}
