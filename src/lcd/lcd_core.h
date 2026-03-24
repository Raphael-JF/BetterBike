#ifndef LCD_CORE__H
#define LCD_CORE__H

#include "rgb_lcd.h"

#define W_gps 15 // 
#define H_gps 16 // 

extern rgb_lcd lcd;
extern uint8_t compass_grid[H_gps][W_gps];
extern double bearing_to_display;

void lcd_respring_gps_status();
void lcd_respring_compass();
void lcd_respring_time();

void clear_compass_for_magnetometer();
void clear_compass_for_gps();
void draw_line(int x0, int y0, int x1, int y1);

#endif // LCD_CORE__H
