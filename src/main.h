#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>


#define M_PI 3.14159265358979323846

#define W 80 // 5*16
#define H 16 // 8*2 

typedef struct {
	uint8_t bits[(W * H + 7) / 8];
} PixelGrid;

extern PixelGrid grid;

void grid_set(int x, int y, uint8_t value);
uint8_t grid_get(int x, int y);


void lcd_respring_compass();
void display_lcd();
#endif // MAIN_H