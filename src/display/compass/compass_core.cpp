#include "compass_core.h"


struct compass_needle_pos compass_pos = {2*H_gps, 2*W_gps};
struct bin_matrix* compass_grid = create_bin_matrix(W_gps, H_gps);
double bearing_to_display = 0.0;


/*
    Extrait une "char" de 5x8 pixels depuis la grille du compas, à la position donnée par x et y (en nombre de caractères, pas de pixels), et la stocke dans out (format attendu par lcd.createChar).
*/
void extract_char(int x, int y, uint8_t out[8]) {
    for (int i = 0; i < 8; i++) {
        out[i] = 0;
        for (int j = 0; j < 5; j++) {
            if (get_pixel_bin_matrix(compass_grid, x*5 + j, y*8 + i)) {
                out[i] |= (1 << (4 - j));
            }
        }
    }
}


void display_refresh_compass() {
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

void highlight_compass_frame() {
    
    set_pixel_bin_matrix(compass_grid, 0, 0, true);
    set_pixel_bin_matrix(compass_grid, 1, 0, true);
    set_pixel_bin_matrix(compass_grid, 0, 1, true);


    set_pixel_bin_matrix(compass_grid, 0, H_gps - 1, true);
    set_pixel_bin_matrix(compass_grid, 0, H_gps - 2, true);
    set_pixel_bin_matrix(compass_grid, 1, H_gps - 1, true);

    set_pixel_bin_matrix(compass_grid, W_gps - 1, H_gps - 1, true);
    set_pixel_bin_matrix(compass_grid, W_gps - 1, H_gps - 2, true);
    set_pixel_bin_matrix(compass_grid, W_gps - 2, H_gps - 1, true);

    set_pixel_bin_matrix(compass_grid, W_gps - 1, 0, true);
    set_pixel_bin_matrix(compass_grid, W_gps - 1, 1, true);
    set_pixel_bin_matrix(compass_grid, W_gps - 2, 0, true);
}


void unhighlight_compass_frame() {
    set_pixel_bin_matrix(compass_grid, 0, 0, false);
    set_pixel_bin_matrix(compass_grid, 1, 0, false);
    set_pixel_bin_matrix(compass_grid, 0, 1, false);


    set_pixel_bin_matrix(compass_grid, 0, H_gps - 1, false);
    set_pixel_bin_matrix(compass_grid, 0, H_gps - 2, false);
    set_pixel_bin_matrix(compass_grid, 1, H_gps - 1, false);

    set_pixel_bin_matrix(compass_grid, W_gps - 1, H_gps - 1, false);
    set_pixel_bin_matrix(compass_grid, W_gps - 1, H_gps - 2, false);
    set_pixel_bin_matrix(compass_grid, W_gps - 2, H_gps - 1, false);

    set_pixel_bin_matrix(compass_grid, W_gps - 1, 0, false);
    set_pixel_bin_matrix(compass_grid, W_gps - 1, 1, false);
    set_pixel_bin_matrix(compass_grid, W_gps - 2, 0, false);
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
            set_pixel_bin_matrix(compass_grid, x, y, false);
        }
    }
}   

void clear_whole_compass(){
      for (int y = 0; y < H_gps; y++) {
        for (int x = 0; x < W_gps; x++) {
            set_pixel_bin_matrix(compass_grid, x, y, false);
        }
    }
}   
