#include <stdint.h>

#include "lcd_sim.h"

extern int x_start, y_start;
extern uint8_t grid[H][W];

void draw_caracter(int x, int y, uint8_t char_data[8]) {
    set_cursor(x, y);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 5; j++) {
            if (char_data[i] & (1 << (4 - j))) {
                grid[y_start + i][x_start + j] = 1;
            }
        }
    }
}