#include <stdint.h>
#include "Arduino.h"
#include "rgb_lcd.h"


#include "lcd/lcd_core.h"
#include "gps/gps_core.h"
#include "time/time_manager.h"

rgb_lcd lcd;
uint8_t compass_grid[H_gps][W_gps];

/*
    Extrait une "char" de 5x8 pixels depuis la grille du compas, à la position donnée par x et y (en nombre de caractères, pas de pixels), et la stocke dans out (format attendu par lcd.createChar).
*/
void extract_char(int x, int y, uint8_t out[8]) {
    for (int i = 0; i < 8; i++) {
        out[i] = 0;
        for (int j = 0; j < 5; j++) {
            if (compass_grid[y*8 + i][x*5 + j]) {
                out[i] |= (1 << (4 - j));
            }
        }
    }
}

void lcd_respring_gps_status() {
    lcd.setCursor(11, 0);
    lcd.write(byte(is_gps_active ? 6 : 7));
}


void lcd_respring_compass() {

    const double cx = W_gps / 2.0;
    const double cy = H_gps / 2.0;

    
    double dx = cos(bearing_to_display);
    double dy = -sin(bearing_to_display);

    const uint8_t tail_length = W_gps < H_gps ? W_gps/2 : H_gps/2;
    const double x = cx + tail_length*dx;
    const double y = cy + tail_length*dy;
    if(bearing_to_display < M_PI / 2) {
        draw_line(cx, cy - 1, x, y);
    }
    else if(bearing_to_display < M_PI) {
        draw_line(cx - 1, cy - 1, x, y);

    }
    else if(bearing_to_display < 3*M_PI/2) {
        draw_line(cx - 1, cy, x, y);
    }
    else{
        draw_line(cx, cy, x, y);
    }

    uint8_t c00[8], c10[8], c20[8];
    uint8_t c01[8], c11[8], c21[8];
    extract_char(0, 0, c00);
    extract_char(1, 0, c10);
    extract_char(2, 0, c20);
    extract_char(0, 1, c01);
    extract_char(1, 1, c11);
    extract_char(2, 1, c21);

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

void clear_compass_for_magnetometer() {
    clear_compass_for_gps();

    const int cx = W_gps / 2;
    const int cy = H_gps / 2;
    
    // N (top)
    compass_grid[2][cx] = 1;
    compass_grid[3][cx] = 1;
    
    // S (bottom)
    compass_grid[H_gps - 4][cx] = 1;
    compass_grid[H_gps - 3][cx] = 1;
    
    // E (right)
    compass_grid[cy][W_gps - 3] = 1;
    compass_grid[cy][W_gps - 2] = 1;
    
    // W (left)
    compass_grid[cy][2] = 1;
    compass_grid[cy][3] = 1;

}

void clear_compass_for_gps() {
    for (int y = 0; y < H_gps; y++) {
        for (int x = 0; x < W_gps; x++) {
            compass_grid[y][x] = 0;
        }
    }
}   

void draw_line(int x0, int y0, int x1, int y1) {
    int dx = fabs(x1 - x0);
    int dy = fabs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1) {
        // allume pixel
        if (x0 >= 0 && x0 < W_gps && y0 >= 0 && y0 < H_gps)
            compass_grid[y0][x0] = 1;

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

