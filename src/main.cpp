#include <math.h>
#include <stdint.h>

#include <Wire.h>
#include "Arduino.h"
#include "rgb_lcd.h"

#include "main.h"
#include "caracters.h"
#include "gps.h"
#include "utc.h"

rgb_lcd lcd;
uint8_t grid[H][W];
int x_start = 0;
int y_start = 0;
float last_angle = -1000;
float angle = 0;

// État visuel GPS (icône validation / négation)
bool is_gps_active = 0;

// Heure actuellement affichée à l'écran (normalisée 0..23)
int hours = -1;
// Minute actuellement affichée à l'écran (normalisée 0..59)
int minutes = -1;

// Dernière heure reçue de la source GPS (ou -1 si aucune donnée)
int fresh_hours = -1;
// Dernière minute reçue de la source GPS (ou -1 si aucune donnée)
int fresh_minutes = -1;



// Dernière valeur GPS prise en compte pour détecter un nouveau timestamp GPS
int last_fresh_hours = -1;
int last_fresh_minutes = -1;

// Base temporelle GPS retenue pour faire avancer l'heure avec millis()
int gps_base_hour = -1;
int gps_base_minutes = -1;
// Valeur de millis() au moment où gps_base_hour/gps_base_minutes ont été synchronisées
unsigned long gps_sync_millis = 0;


int utc_year = 2026;
int utc_month = 4;
int utc_day = 21;


void lcd_respring_gps_status() {
    lcd.setCursor(10, 0);
    lcd.write(byte(is_gps_active ? 6 : 7));
}



void lcd_respring_time(int new_hours, int new_minutes) {
    new_hours = (new_hours % 24 + 24) % 24;
    new_minutes = (new_minutes % 60 + 60) % 60;

    if (hours == new_hours && minutes == new_minutes) {
        return;
    }

    hours = new_hours;
    minutes = new_minutes;
    // if(myGPS.available()){
        // utc_year = myGPS.utc_year;
        // utc_month = myGPS.utc_month;
        // utc_day = myGPS.utc_day;
    // }   
    
    int hours_local = hours;
    int minutes_local = minutes;
    utc_to_local(utc_day, utc_month, utc_year, hours, minutes, &hours_local, &minutes_local);
    char time_str[6];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", hours_local, minutes_local);
    lcd.setCursor(0, 0);
    lcd.print(time_str);
}



void update_time() {
    if (fresh_hours != -1 && fresh_minutes != -1) {
        if (fresh_hours != last_fresh_hours || fresh_minutes != last_fresh_minutes) {
            gps_base_hour = fresh_hours;
            gps_base_minutes = fresh_minutes;
            gps_sync_millis = millis();
            last_fresh_hours = fresh_hours;
            last_fresh_minutes = fresh_minutes;
        }

        if (gps_base_hour != -1 && gps_base_minutes != -1) {
            unsigned long elapsed_minutes = (millis() - gps_sync_millis) / 60000UL;
            int base_total_minutes = gps_base_hour * 60 + gps_base_minutes;
            int current_total_minutes = (base_total_minutes + (int)elapsed_minutes) % (24 * 60);

            int current_hours = current_total_minutes / 60;
            int current_minutes = current_total_minutes % 60;
            lcd_respring_time(current_hours, current_minutes);
        }
        return;
    }

    last_fresh_hours = -1;
    last_fresh_minutes = -1;

    // unsigned long total_minutes = millis() / 60000;
    // int current_hours = (total_minutes / 60) % 24;
    // int current_minutes = total_minutes % 60;
    // lcd_respring_time(current_hours, current_minutes);
}




void lcd_respring_compass() {
    uint8_t c00[8], c10[8], c20[8];
    uint8_t c01[8], c11[8], c21[8];
    extract_char(13, 0, c00);
    extract_char(14, 0, c10);
    extract_char(15, 0, c20);
    extract_char(13, 1, c01);
    extract_char(14, 1, c11);
    extract_char(15, 1, c21);
    Serial.println(c00[0], HEX);


    lcd.createChar(0, c00);
    lcd.createChar(1, c10);
    lcd.createChar(2, c20);
    lcd.createChar(3, c01);
    lcd.createChar(4, c11);
    lcd.createChar(5, c21);
}



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
        Serial.print("Angle: ");
        Serial.println(angle);
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

    

    update_time();
    
    

    // Serial.println("Starting LCD simulation...");

    delay(500);
    is_gps_active = 1;
    fresh_hours = 12;
    fresh_minutes = 34;
}