#include <stdint.h>
#include "Arduino.h"
#include "blinking.h"
#include "lcd/lcd_core.h"

blinking* blinkings_to_update[] = {
    NULL, // IDX_COMPASS_FRAME_BLINKING : clignotement du cadre de la boussole en cas de non-fix GPS
};
blinking compass_frame_blinking;


struct blinking blinking_create(uint16_t period, uint8_t initial_state) {
    struct blinking b;
    b.state = initial_state;
    b.period = period;
    b.last_toggle_millis = 0;
    return b;
}

uint8_t update_one_blinking(struct blinking* b) {
    unsigned long current_millis = millis();
    if (current_millis - b->last_toggle_millis >= b->period) {
        b->state = !b->state; // toggle state
        b->last_toggle_millis = current_millis;
        return 1;
    } else {
        return 0;
    }
}

void update_all_blinkings() {
    blinking *_compass_blinking = blinkings_to_update[IDX_COMPASS_FRAME_BLINKING];

    // Clignotement du cadre de la boussole si pas de fix GPS
    if(_compass_blinking != NULL && update_one_blinking(_compass_blinking)) {
        if (_compass_blinking->state) {
            highlight_compass_frame();
        }
        else{
            clear_compass_frame();
            calculate_gps_grid();
        }
        lcd_respring_compass();
    }
}