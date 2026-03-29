#include <math.h>
#include <stdint.h>
#include <string.h>

#include <Wire.h>
#include "Arduino.h"


#include "gps/gps_core.h"
#include "time/time_manager.h"
#include "lcd/lcd_core.h"
#include "bluetooth/bluetooth.h"







void setup() {
    Serial.begin(115200);

    Wire.begin(21, 22); // SDA, SCL
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); // UART1

    // set up the lcd's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setRGB(127, 127, 127);

    ble.begin("VeloGPS");


    lcd.setCursor(0, 0);
    lcd.print("--:--  GPS");

    // uint8_t gps_valid_char[8] = {
    //     0b00000,
    //     0b00001,
    //     0b00010,
    //     0b10100,
    //     0b01000,
    //     0b00000,
    //     0b00000,
    //     0b00000
    // };

    // uint8_t gps_invalid_char[8] = {
    //     0b00000,
    //     0b10001,
    //     0b01010,
    //     0b00100,
    //     0b01010,
    //     0b10001,
    //     0b00000,
    //     0b00000
    // };

    // logo bluetooth :
    uint8_t bluetooth_car[8] = {
        0b00110,
        0b10101,
        0b01101,
        0b00110,
        0b01101,
        0b10101,
        0b00110,
        0b00000
    };

    lcd.createChar(6, bluetooth_car);
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
    update_blinkings();
    // 1. Toujours lire le GPS
    while (gpsSerial.available()) {
        // Serial.println("Tic");
        gps.encode(gpsSerial.read());
    }
    // Serial.println("Tac");

    const bool gps_time_fresh = gps.time.isValid() && gps.time.isUpdated() && gps.time.age() < 2000;
    const bool gps_date_fresh = gps.date.isValid() && gps.date.isUpdated() && gps.date.age() < 2000;
    const bool gps_fix_active = gps.location.isValid() && gps.location.age() < 2000;

    is_gps_active = gps_time_fresh && gps_date_fresh && gps_fix_active;

    // 2. Utiliser les données seulement si dispo
    if (gps.location.isValid()) {
        // Serial.println(gps.location.lat(), 6);
        // Serial.println(gps.location.lng(), 6);
    }

    if (gps_time_fresh) {
        last_got_gps_time.hours = gps.time.hour();
        last_got_gps_time.minutes = gps.time.minute();
        
    }
    if (gps_date_fresh) {
        utc_day = gps.date.day();
        utc_month = gps.date.month();
        utc_year = gps.date.year();
    }



    bearing_to_display += 0.25;
    if(bearing_to_display > 2*M_PI) {
        bearing_to_display -= 2*M_PI;
    }


    if (fabs(bearing_to_display - last_bearing_to_display) > 0.05) {
        if (is_gps_active) {
            clear_compass_frame();
        }
        else{
            highlight_compass_frame();
        }
        lcd_respring_compass();
        last_bearing_to_display = bearing_to_display;
    }

    
    struct time_base old_time = current_time;
    update_time();
    if (current_time.minutes != old_time.minutes || current_time.hours != old_time.hours){
        lcd_respring_time();
    }
     
    while (ble.available()) {
        char c = ble.read();
        Serial.write(c); // debug brut
    }
        // ici tu parses lat/lon


    delay(250);
    // last_got_gps_time = {12, 34};
}