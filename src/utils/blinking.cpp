#include <stdint.h>
#include "Arduino.h"
#include "blinking.h"

blinking* active_blinkings[] = {
    NULL, // IDX_COMPASS_FRAME_BLINKING : clignotement du cadre de la boussole en cas de non-fix GPS
};


struct blinking blinking_create(uint16_t period, uint8_t initial_state) {
    struct blinking b;
    b.initial_state = initial_state;
    b.state = initial_state;
    b.period = period;
    b.last_toggle_millis = 0;
    b.just_started = 0;
    return b;
}



void blinking_stop(uint8_t index) {
    active_blinkings[index] = NULL;
}

void blinking_start(uint8_t index, struct blinking* b) {
    b->state = b->initial_state;
    b->last_toggle_millis = millis();
    b->just_started = 1;
    active_blinkings[index] = b;
}

uint8_t blinking_respring(uint8_t index) {
    struct blinking* b = active_blinkings[index];
    if (b == NULL) {
        return 0;
    }
    unsigned long current_millis = millis();
    unsigned long elapsed = current_millis - b->last_toggle_millis;
    if (elapsed >= b->period) {
        unsigned long toggles = elapsed / b->period;
        if ((toggles & 1UL) != 0UL) {
            b->state = !b->state;
            b->last_toggle_millis += toggles * b->period;
            return 1;
        }
        b->last_toggle_millis += toggles * b->period;
    } else if(b->just_started) {
        b->just_started = 0;
        return 1;
    }
    return 0;
}

