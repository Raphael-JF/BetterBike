#include <math.h>
#include <stdint.h>

#include <Wire.h>
#include "Arduino.h"

#include "main.h"
#include "caracters.h"
#include "gps/gps_core.h"
#include "gps/gps_draw.h"
#include "time/time_manager.h"
#include "lcd/lcd_core.h"

float last_angle = -1000;
float angle = 0;








void setup() {
    // Serial.begin(115200); // esp
    Serial.begin(9600); // arduino
    // Wire.begin();
    // set up the lcd's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setRGB(127, 127, 127);


    lcd.setCursor(0, 0);
    lcd.print("--:-- GPS");

    uint8_t gps_valid_char[8] = {
        0b00000,
        0b00001,
        0b00010,
        0b10100,
        0b01000,
        0b00000,
        0b00000,
        0b00000
    };

    uint8_t gps_invalid_char[8] = {
        0b00000,
        0b10001,
        0b01010,
        0b00100,
        0b01010,
        0b10001,
        0b00000,
        0b00000
    };

    lcd.createChar(6, gps_valid_char);
    lcd.createChar(7, gps_invalid_char);
    lcd_respring_gps_status();

    lcd.setCursor(12, 0);
    lcd.print("|");
    lcd.setCursor(12, 1);
    lcd.print("|");

    lcd.setCursor(13, 0);

    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.setCursor(13, 1);
    lcd.write(byte(3));
    lcd.write(byte(4));
    lcd.write(byte(5));

    update_compass(0);
    lcd_respring_compass();


}


void loop() {

    angle += 0.5;
    if(angle > 2*M_PI) {
        angle -= 2*M_PI;
    }


    if (fabs(angle - last_angle) > 0.05) {
        if (is_gps_active) {
            clear_compass_for_gps();
        }
        else{
            clear_compass_for_magnetometer();
        }
        update_compass(angle);
        lcd_respring_compass();
        last_angle = angle;
    }

    
    struct time_base old_time = current_time;
    update_time();
    if (current_time.minutes != old_time.minutes || current_time.hours != old_time.hours){
        lcd_respring_time();
    }

    // Serial.println("Starting LCD simulation...");

    delay(500);
    is_gps_active = 1;
    last_got_gps_time = {12, 34};
}