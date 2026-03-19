#ifndef ROTATION_FLECHE_H
#define ROTATION_FLECHE_H

#include <stdint.h>


#define M_PI       3.14159265358979323846

#define W 80 // 5*16
#define H 16 // 8*2 

extern uint8_t grid[H][W];


void set_cursor(int x, int y);
void display_lcd();
#endif // ROTATION_FLECHE_H