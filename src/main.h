#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>


#define M_PI 3.14159265358979323846

#define W 80 // 5*16
#define H 16 // 8*2 

extern uint8_t grid[H][W];


void lcd_respring_compass();
void display_lcd();
#endif // MAIN_H