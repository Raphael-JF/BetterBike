#include <math.h>
#include <stdint.h>
#include <string.h>

#include <Wire.h>
#include "Arduino.h"
#include "rgb_lcd.h"

#include "main.h"
#include "gps.h"

rgb_lcd lcd;
PixelGrid grid;
int x_start = 0;
int y_start = 0;

typedef struct {
    int sx;
    int sy;
    int ex;
    int ey;
} CompassSignature;

CompassSignature last_signature = {0, 0, 0, 0};

static CompassSignature build_compass_signature(float angle) {
    const float cx = W_gps / 2.0;
    const float cy = H_gps / 2.0;
    const int tail_length = W_gps < H_gps ? W_gps / 2 : H_gps / 2;

    const float dx = cos(angle);
    const float dy = -sin(angle);
    const int ex = (int)(cx + tail_length * dx);
    const int ey = (int)(cy + tail_length * dy);

    int sx;
    int sy;
    if (angle < M_PI / 2) {
        sx = (int)cx;
        sy = (int)cy - 1;
    } else if (angle < M_PI) {
        sx = (int)cx - 1;
        sy = (int)cy - 1;
    } else if (angle < 3 * M_PI / 2) {
        sx = (int)cx - 1;
        sy = (int)cy;
    } else {
        sx = (int)cx;
        sy = (int)cy;
    }

    CompassSignature signature = {sx, sy, ex, ey};
    return signature;
}

static bool same_signature(CompassSignature a, CompassSignature b) {
    return a.sx == b.sx && a.sy == b.sy && a.ex == b.ex && a.ey == b.ey;
}

void grid_set(int x, int y, uint8_t value) {
    if (x < 0 || x >= W || y < 0 || y >= H) {
        return;
    }

    const int index = y * W + x;
    const int byte_index = index >> 3;
    const uint8_t mask = (uint8_t)(1u << (index & 7));

    if (value) {
        grid.bits[byte_index] |= mask;
    } else {
        grid.bits[byte_index] &= (uint8_t)~mask;
    }
}

uint8_t grid_get(int x, int y) {
    if (x < 0 || x >= W || y < 0 || y >= H) {
        return 0;
    }

    const int index = y * W + x;
    const int byte_index = index >> 3;
    const uint8_t mask = (uint8_t)(1u << (index & 7));
    return (grid.bits[byte_index] & mask) ? 1 : 0;
}


void set_cursor(int x, int y){
    x_start = x * 5;
    y_start = y * 8;
}

void draw_caracter(int x, int y, uint8_t char_data[8]) {
    set_cursor(x, y);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 5; j++) {
            if (char_data[i] & (1 << (4 - j))) {
                grid_set(x_start + j, y_start + i, 1);
            }
        }
    }
}

void extract_char(int x, int y, uint8_t out[8]) {
    for (int i = 0; i < 8; i++) {
        out[i] = 0;
        for (int j = 0; j < 5; j++) {
            if (grid_get(x * 5 + j, y * 8 + i)) {
                out[i] |= (1 << (4 - j));
            }
        }
    }
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
    last_signature = build_compass_signature(0);

}

void lcd_respring_compass() {
    uint8_t current_chars[6][8];
    extract_char(13, 0, current_chars[0]);
    extract_char(14, 0, current_chars[1]);
    extract_char(15, 0, current_chars[2]);
    extract_char(13, 1, current_chars[3]);
    extract_char(14, 1, current_chars[4]);
    extract_char(15, 1, current_chars[5]);

    const int char_x[6] = {13, 14, 15, 13, 14, 15};
    const int char_y[6] = {0, 0, 0, 1, 1, 1};

    for (int i = 0; i < 6; i++) {
        const bool changed = memcmp(current_chars[i], last_chars[i], 8) != 0;
        if (!changed) {
            continue;
        }

        lcd.createChar(i, current_chars[i]);
        lcd.setCursor(char_x[i], char_y[i]);
        lcd.write((uint8_t)i);
        memcpy(last_chars[i], current_chars[i], 8);
    }
}


float angle = 0;
void loop() {

    angle = angle + 0.1;
    if(angle > 2*M_PI) {
        angle = 0;
    }
    

    const CompassSignature signature = build_compass_signature(angle);
    if (!same_signature(signature, last_signature)) {
        update_compass(angle);
        lcd_respring_compass();
        last_signature = signature;
    }

    delay(500);
}
/*********************************************************************************************************
    END FILE
*********************************************************************************************************/