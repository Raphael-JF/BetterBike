#include <math.h>
#include <stdint.h>

#include <Wire.h>
#include "Arduino.h"


#include "main.h"
#include "gps/gps_core.h"
#include "time/time_manager.h"
#include "lcd/lcd_core.h"











void setup() {
    // Serial.begin(115200); // esp
    Serial.begin(9600); // arduino
    gpsSerial.begin(9600); // GPS

    // set up the lcd's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setRGB(127, 127, 127);


    lcd.setCursor(0, 0);
    lcd.print("--:--  GPS");

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

    lcd.setCursor(13, 0);

    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.setCursor(13, 1);
    lcd.write(byte(3));
    lcd.write(byte(4));
    lcd.write(byte(5));

    lcd_respring_compass();


}


void loop() {

    // 1. Toujours lire le GPS
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    // 2. Utiliser les données seulement si dispo
    if (gps.location.isUpdated()) {
        Serial.println(gps.location.lat(), 6);
    }


    bearing_to_display += 0.25;
    if(bearing_to_display > 2*M_PI) {
        bearing_to_display -= 2*M_PI;
    }


    if (fabs(bearing_to_display - last_bearing_to_display) > 0.05) {
        if (is_gps_active) {
            clear_compass_for_gps();
        }
        else{
            clear_compass_for_magnetometer();
        }
        lcd_respring_compass();
        last_bearing_to_display = bearing_to_display;
    }

    
    struct time_base old_time = current_time;
    update_time();
    if (current_time.minutes != old_time.minutes || current_time.hours != old_time.hours){
        lcd_respring_time();
    }
     

    delay(50);
    is_gps_active = 1;
    // last_got_gps_time = {12, 34};
}