#ifndef LCD_CORE__H
#define LCD_CORE__H

#include "rgb_lcd.h"

#define W 80 // 5*16
#define H 16 // 8*2 

extern rgb_lcd lcd;
extern uint8_t grid[H][W];

void lcd_respring_gps_status();
void lcd_respring_compass();

/*
    Always displays the time in the lcd as given in parameters.
*/
void lcd_respring_time();

#endif // LCD_CORE__H
