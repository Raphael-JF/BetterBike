#include <stdint.h>
#include "Arduino.h"
#include "rgb_lcd.h"


#include "lcd/lcd_core.h"
#include "gps/gps_core.h"
#include "time/time_manager.h"

rgb_lcd lcd;
uint8_t grid[H][W];


void extract_char(int x, int y, uint8_t out[8]) {
    for (int i = 0; i < 8; i++) {
        out[i] = 0;
        for (int j = 0; j < 5; j++) {
            if (grid[y*8 + i][x*5 + j]) {
                out[i] |= (1 << (4 - j));
            }
        }
    }
}

void lcd_respring_gps_status() {
    lcd.setCursor(10, 0);
    lcd.write(byte(is_gps_active ? 6 : 7));
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

    lcd.createChar(0, c00);
    lcd.createChar(1, c10);
    lcd.createChar(2, c20);
    lcd.createChar(3, c01);
    lcd.createChar(4, c11);
    lcd.createChar(5, c21);
}




void lcd_respring_time() {
    char time_str[6];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", current_time.hours, current_time.minutes);
    lcd.setCursor(0, 0);
    lcd.print(time_str);
}