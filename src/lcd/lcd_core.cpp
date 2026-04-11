#include <stdint.h>
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

#include "lcd/lcd_core.h"
#include "lcd/binary_matrix.h"
#include "gps/gps_core.h"
#include "time/time_manager.h"
#include "magnetometer/magnetometer.h"

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
struct compass_pos compass_pos = {2*H_gps, 2*W_gps};
struct bin_matrix* compass_grid = create_bin_matrix(W_gps, H_gps);
double bearing_to_display = 0.0;


/*
    Extrait une "char" de 5x8 pixels depuis la grille du compas, à la position donnée par x et y (en nombre de caractères, pas de pixels), et la stocke dans out (format attendu par lcd.createChar).
*/
void extract_char(int x, int y, uint8_t out[8]) {
    for (int i = 0; i < 8; i++) {
        out[i] = 0;
        for (int j = 0; j < 5; j++) {
            if (get_pixel(compass_grid, x*5 + j, y*8 + i)) {
                out[i] |= (1 << (4 - j));
            }
        }
    }
}

void lcd_respring_gps_status() {
    lcd.setCursor(6, 0);
    lcd.print("no gps");
}


uint8_t calculate_compass_grid(){
    double math_compass_angle = M_PI_2 - bearing_to_display;

    math_compass_angle = fmod(math_compass_angle, 2 * M_PI);
    if (math_compass_angle < 0) {
        math_compass_angle += 2 * M_PI;
    }

    double dx = cos(math_compass_angle);
    double dy = -sin(math_compass_angle);

    uint8_t x = CX + round((double)(ARROW_LENGTH)*dx);
    uint8_t y = CY + round((double)(ARROW_LENGTH)*dy);

    if(x == compass_pos.x && y == compass_pos.y) {
        return 0; // Indique que l'aiguille n'a pas besoin d'être redessinée
    }

    clear_inner_compass();

    compass_pos.x = x;
    compass_pos.y = y;
    uint8_t cx = (uint8_t)CX;
    uint8_t cy = (uint8_t)CY;


    if(math_compass_angle < M_PI / 2) {
        draw_line(cx, cy - 1, x, y - 1);
    }
    else if(math_compass_angle < M_PI) {
        draw_line(cx - 1, cy - 1, x - 1, y - 1);

    }
    else if(math_compass_angle < 3*M_PI/2) {
        draw_line(cx - 1, cy, x - 1, y);
    }
    else{
        draw_line(cx, cy, x, y);
    }
    return 1;
}

void lcd_respring_compass() {
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

void highlight_compass_frame() {
    
    set_pixel(compass_grid, 0, 0, true);
    set_pixel(compass_grid, 1, 0, true);
    set_pixel(compass_grid, 0, 1, true);


    set_pixel(compass_grid, 0, H_gps - 1, true);
    set_pixel(compass_grid, 0, H_gps - 2, true);
    set_pixel(compass_grid, 1, H_gps - 1, true);

    set_pixel(compass_grid, W_gps - 1, H_gps - 1, true);
    set_pixel(compass_grid, W_gps - 1, H_gps - 2, true);
    set_pixel(compass_grid, W_gps - 2, H_gps - 1, true);

    set_pixel(compass_grid, W_gps - 1, 0, true);
    set_pixel(compass_grid, W_gps - 1, 1, true);
    set_pixel(compass_grid, W_gps - 2, 0, true);
    


}


void unhighlight_compass_frame() {
    set_pixel(compass_grid, 0, 0, false);
    set_pixel(compass_grid, 1, 0, false);
    set_pixel(compass_grid, 0, 1, false);


    set_pixel(compass_grid, 0, H_gps - 1, false);
    set_pixel(compass_grid, 0, H_gps - 2, false);
    set_pixel(compass_grid, 1, H_gps - 1, false);

    set_pixel(compass_grid, W_gps - 1, H_gps - 1, false);
    set_pixel(compass_grid, W_gps - 1, H_gps - 2, false);
    set_pixel(compass_grid, W_gps - 2, H_gps - 1, false);

    set_pixel(compass_grid, W_gps - 1, 0, false);
    set_pixel(compass_grid, W_gps - 1, 1, false);
    set_pixel(compass_grid, W_gps - 2, 0, false);
    


}

void clear_inner_compass() {
    for (int y = 0; y < H_gps; y++) {
        for (int x = 0; x < W_gps; x++) {
            if ((x == 0 && (y == 0 || y == 1 || y == H_gps - 1 || y == H_gps - 2)) ||
                (x == 1 && (y == 0 || y == H_gps - 1)) ||
                (x == W_gps - 1 && (y == 0 || y == 1 || y == H_gps - 1 || y == H_gps - 2)) ||
                (x == W_gps - 2 && (y == 0 || y == H_gps - 1))) {
                continue; // ne pas effacer les pixels du cadre
            }
            set_pixel(compass_grid, x, y, false);
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
            set_pixel(compass_grid, x0, y0, true);

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