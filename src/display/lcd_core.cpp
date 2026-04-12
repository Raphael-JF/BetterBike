#include <stdint.h>
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

#include "display/lcd_core.h"
#include "gps/gps_core.h"

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

uint8_t current_view = NO_VIEW;



void lcd_load_view(uint8_t view) {
    switch (view) {
        case COMPASS_VIEW:
            lcd.setCursor(6, 0);
            lcd.print("no gps");
            break;
        case GPS_STATUS_VIEW:
            // afficher en haut la vitesse, et en bas la distance
            break;
        case CALIBRATION_VIEW:
            // afficher en haut le pourcentage de la calibration du magnétomètre, et en bas le message "calibrating" ou "calibrated"
            lcd.setCursor(1, 1);
            lcd.print("calibrating");
            break;
        case NO_VIEW:
            break;
    }
    current_view = view;
}












