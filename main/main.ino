#include <Wire.h>
#include "rgb_lcd.h"
#include "rotation_fleche.h"

rgb_lcd lcd;

// make some custom characters:
const byte heart[8] PROGMEM = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
};

void setup() {
    // set up the lcd's number of columns and rows:
    lcd.begin(16, 2);

    update_arrow(0);

}



void update_arrow(float angle) {

    dessine_fleche(angle);

    uint8_t c0[8], c1[8], c2[8], c3[8];

    extract_char(0, 0, c0);
    extract_char(5, 0, c1);
    extract_char(0, 8, c2);
    extract_char(5, 8, c3);

    lcd.createChar(0, c0);
    lcd.createChar(1, c1);
    lcd.createChar(2, c2);
    lcd.createChar(3, c3);

    lcd.setCursor(0, 0);
    lcd.write(byte(0));
    lcd.write(byte(1));

    lcd.setCursor(0, 1);
    lcd.write(byte(2));
    lcd.write(byte(3));
}

float last_angle = -1000;

void loop() {

    float angle = millis() / 1000.0 * 2 * PI / 10.0;

    if (fabs(angle - last_angle) > 0.05) {

        update_arrow(angle);

        last_angle = angle;
    }

    delay(50);
}
/*********************************************************************************************************
    END FILE
*********************************************************************************************************/