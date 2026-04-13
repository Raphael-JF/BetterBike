#include <stdint.h>
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

#include "display/lcd_core.h"
#include "display/compass.h"
#include "display/clock.h"
#include "gps/gps_core.h"

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

enum view_idx current_view = NO_VIEW;
struct view* active_view;


void compass_title_on_enter(void) {
    lcd.setCursor(6, 0);
    lcd.print("no gps");
}

uint8_t compass_title_update(void) {
    return 0;
}

void compass_title_render(void) {}


void calibration_text_on_enter(void) {
    lcd.setCursor(0, 1);
    lcd.print("            ");
    lcd.setCursor(1, 1);
    lcd.print("calibrating");
}

uint8_t calibration_text_update(void) {
    return 0;
}

void calibration_text_render(void) {}


struct component calibration_components[] = {Clock, Compass};
struct component compass_components[] = {Clock, Compass};
struct component gps_components[] = {Clock, Compass};
struct view calibration_view = {2, calibration_components};
struct view compass_view = {2, compass_components};
struct view gps_view = {2, gps_components};
struct view empty_view = {0, nullptr};


struct view* map_view(enum view_idx view) {
    switch (view) {
        case CALIBRATION_VIEW:
            return &calibration_view;
        case COMPASS_VIEW:
            return &compass_view;
        case GPS_VIEW:
                return &gps_view; // la vue GPS est identique à la vue boussole, mais avec un titre différent
        case NO_VIEW:
        default:
            return &empty_view;
    }
}



void lcd_load_view(enum view_idx view) {
    active_view = map_view(view);
    for (uint8_t i = 0; i < active_view->num_components; i++) {
        active_view->components[i].onEnter();
    }

    current_view = view;
}

void lcd_update_current_view(void) {
    for (uint8_t i = 0; i < active_view->num_components; i++) {
        active_view->components[i].update();
    }
}












