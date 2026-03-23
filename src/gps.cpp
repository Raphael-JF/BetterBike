#include "gps.h"
#include "lcd_sim.h"
#include <math.h>
#include "stdint.h"
#include "Arduino.h"
#include "main.h"


void draw_line(int x0, int y0, int x1, int y1) {
    int dx = fabs(x1 - x0);
    int dy = fabs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1) {
        // allume pixel
        if (x0 >= 0 && x0 < W_gps && y0 >= 0 && y0 < H_gps)
            grid[y0][x0 + 13*5] = 1;

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}


void clear_compass_for_gps() {
    for (int y = 0; y < H_gps; y++) {
        for (int x = 0; x < W_gps; x++) {
            grid[y][x + 13*5] = 0;
        }
    }
}


    

void clear_compass_for_magnetometer() {
    for (int y = 0; y < H_gps; y++) {
        for (int x = 0; x < W_gps; x++) {
            grid[y][x + 13*5] = 0;
        }
    }
    const int offset = 13 * 5;
    const int cx = W_gps / 2;
    const int cy = H_gps / 2;
    
    // N (top)
    grid[2][cx + offset] = 1;
    grid[3][cx + offset] = 1;
    
    // S (bottom)
    grid[H_gps - 4][cx + offset] = 1;
    grid[H_gps - 3][cx + offset] = 1;
    
    // E (right)
    grid[cy][W_gps - 3 + offset] = 1;
    grid[cy][W_gps - 2 + offset] = 1;
    
    // W (left)
    grid[cy][2 + offset] = 1;
    grid[cy][3 + offset] = 1;

}


void update_compass(float angle) {

    const float cx = W_gps / 2.0;
    const float cy = H_gps / 2.0;

    
    float dx = cos(angle);
    float dy = -sin(angle);

    const int tail_length = W_gps < H_gps ? W_gps/2 : H_gps/2;
    const float x = cx + tail_length*dx;
    const float y = cy + tail_length*dy;
    if(angle < M_PI/2) {
        draw_line(cx, cy - 1, x, y);
    }
    else if(angle < M_PI) {
        draw_line(cx - 1, cy - 1, x, y);

    }
    else if(angle < 3*M_PI/2) {
        draw_line(cx - 1, cy, x, y);
    }
    else{
        draw_line(cx, cy, x, y);
    }

}