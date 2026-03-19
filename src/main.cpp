#include <math.h>
#include <stdint.h>

#include <Wire.h>
#include "Arduino.h"
#include "rgb_lcd.h"

#include "main.h"
#include "caracters.h"
#include "gps.h"

rgb_lcd lcd;
uint8_t grid[H][W];
int x_start = 0;
int y_start = 0;


void set_cursor(int x, int y){
    x_start = x * 5;
    y_start = y * 8;
}


void setup() {
    // Serial.begin(115200); // esp
    Serial.begin(9600); // arduino
    // Wire.begin();
    // set up the lcd's number of columns and rows:
    Serial.println("Starting LCD simulation...");
    lcd.begin(16, 2);
    lcd.setRGB(255, 255, 255);

    // lcd.print("Hello World!");

    update_compass(0);
    lcd_respring_compass();

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
    Serial.print("c00 : ");
    for (int i = 0; i < 8; i++) {
        Serial.print(c00[i], HEX);
    }


    lcd.createChar(0, c00);
    lcd.createChar(1, c10);
    lcd.createChar(2, c20);
    lcd.createChar(3, c01);
    lcd.createChar(4, c11);
    lcd.createChar(5, c21);

    lcd.setCursor(13, 0);

    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.setCursor(13, 1);
    lcd.write(byte(3));
    lcd.write(byte(4));
    lcd.write(byte(5));
}


float last_angle = -1000;

void loop() {

    float angle = millis() / 1000.0 * 2 * M_PI / 10; // 1 tour toutes les 10 secondes
    

    if (fabs(angle - last_angle) > 0.05) {
        Serial.print("Angle: ");
        Serial.println(angle);
        update_compass(angle);
        lcd_respring_compass();
        last_angle = angle;
    }
    // Serial.println("Starting LCD simulation...");

    delay(500);
}
/*********************************************************************************************************
    END FILE
*********************************************************************************************************/