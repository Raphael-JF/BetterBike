#include <stdint.h>
#include "Arduino.h"
#include "blinking.h"


struct blinking blinking_create(uint16_t period, uint8_t initial_state) {
    struct blinking b;
    b.state = initial_state;
    b.period = period;
    b.last_toggle_millis = 0;
    return b;
}

uint8_t blinking_update(struct blinking* b) {
    unsigned long current_millis = millis();
    if (current_millis - b->last_toggle_millis >= b->period) {
        b->state = !b->state; // toggle state
        b->last_toggle_millis = current_millis;
        return 1;
    } else {
        return 0;
    }
}