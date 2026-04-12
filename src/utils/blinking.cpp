#include <stdint.h>
#include "Arduino.h"
#include "blinking.h"



struct blinking blinking_create(uint16_t period, uint8_t initial_blink_state) {
    struct blinking b;
    b.working = 0;
    b.initial_blink_state = initial_blink_state;
    b.blink_state = initial_blink_state;
    b.period = period;
    b.last_toggle_millis = 0;
    b.just_started = 0;
    return b;
}



void blinking_stop(struct blinking b) {
    b.working = 0;
}

void blinking_start(struct blinking b) {
    b.blink_state = b.initial_blink_state;
    b.last_toggle_millis = millis();
    b.just_started = 1;
    b.working = 1;
}

enum blinking_response blinking_update(struct blinking b) {
    if(!b.working) {
        return BLINKING_NO_CHANGE;
    }
    unsigned long elapsed = millis() - b.last_toggle_millis;
    if (elapsed >= b.period) {
        unsigned long toggles = elapsed / b.period;
        if ((toggles & 1UL) != 0UL) {
            b.blink_state = !b.blink_state;
            b.last_toggle_millis += toggles * b.period;
            return b.blink_state ? BLINKING_STATE_ON : BLINKING_STATE_OFF;
        }
        b.last_toggle_millis += toggles * b.period;
    } else if(b.just_started) {
        b.just_started = 0;
        return b.blink_state ? BLINKING_STATE_ON : BLINKING_STATE_OFF;
    }
    return BLINKING_NO_CHANGE;
}

