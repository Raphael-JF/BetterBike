#include "display/compass/compass_core.h"


struct compass_needle_pos needle_pos = {2*H_COMPASS, 2*W_COMPASS};
struct bin_matrix* compass_grid = create_bin_matrix(W_COMPASS, H_COMPASS);
double needle_bearing = 0.0;


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


    set_pixel_bin_matrix(compass_grid, 0, H_COMPASS - 1, true);
    set_pixel_bin_matrix(compass_grid, 0, H_COMPASS - 2, true);
    set_pixel_bin_matrix(compass_grid, 1, H_COMPASS - 1, true);

    set_pixel_bin_matrix(compass_grid, W_COMPASS - 1, H_COMPASS - 1, true);
    set_pixel_bin_matrix(compass_grid, W_COMPASS - 1, H_COMPASS - 2, true);
    set_pixel_bin_matrix(compass_grid, W_COMPASS - 2, H_COMPASS - 1, true);

    set_pixel_bin_matrix(compass_grid, W_COMPASS - 1, 0, true);
    set_pixel_bin_matrix(compass_grid, W_COMPASS - 1, 1, true);
    set_pixel_bin_matrix(compass_grid, W_COMPASS - 2, 0, true);
}


void unhighlight_compass_frame() {
    set_pixel_bin_matrix(compass_grid, 0, 0, false);
    set_pixel_bin_matrix(compass_grid, 1, 0, false);
    set_pixel_bin_matrix(compass_grid, 0, 1, false);


    set_pixel_bin_matrix(compass_grid, 0, H_COMPASS - 1, false);
    set_pixel_bin_matrix(compass_grid, 0, H_COMPASS - 2, false);
    set_pixel_bin_matrix(compass_grid, 1, H_COMPASS - 1, false);

    set_pixel_bin_matrix(compass_grid, W_COMPASS - 1, H_COMPASS - 1, false);
    set_pixel_bin_matrix(compass_grid, W_COMPASS - 1, H_COMPASS - 2, false);
    set_pixel_bin_matrix(compass_grid, W_COMPASS - 2, H_COMPASS - 1, false);

    set_pixel_bin_matrix(compass_grid, W_COMPASS - 1, 0, false);
    set_pixel_bin_matrix(compass_grid, W_COMPASS - 1, 1, false);
    set_pixel_bin_matrix(compass_grid, W_COMPASS - 2, 0, false);
}

void clear_inner_compass_grid() {
    for (int y = 0; y < H_COMPASS; y++) {
        for (int x = 0; x < W_COMPASS; x++) {
            if ((x == 0 && (y == 0 || y == 1 || y == H_COMPASS - 1 || y == H_COMPASS - 2)) ||
                (x == 1 && (y == 0 || y == H_COMPASS - 1)) ||
                (x == W_COMPASS - 1 && (y == 0 || y == 1 || y == H_COMPASS - 1 || y == H_COMPASS - 2)) ||
                (x == W_COMPASS - 2 && (y == 0 || y == H_COMPASS - 1))) {
                continue; // ne pas effacer les pixels du cadre
            }
            set_pixel_bin_matrix(compass_grid, x, y, false);
        }
    }
}   

void clear_whole_compass(){
      for (int y = 0; y < H_COMPASS; y++) {
        for (int x = 0; x < W_COMPASS; x++) {
            set_pixel_bin_matrix(compass_grid, x, y, false);
        }
    }
}   



/**
 * @brief Bresenham's line algorithm to draw a line between two points in `compass_grid`.
 * The points must be within the bounds of the grid.
 * @param x0 Starting point x coordinate (in pixels)
 * @param y0 Starting point y coordinate (in pixels)
 * @param x1 Ending point x coordinate (in pixels)
 * @param y1 Ending point y coordinate (in pixels)
 */
void draw_line(int x0, int y0, int x1, int y1){
    int dx = fabs(x1 - x0);
    int dy = fabs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1) {
        // allume pixel
        if (x0 >= 0 && x0 < W_COMPASS && y0 >= 0 && y0 < H_COMPASS)
            set_pixel_bin_matrix(compass_grid, x0, y0, true);

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





uint8_t update_needle_position() {

    double dx = cos(needle_bearing);
    double dy = -sin(needle_bearing);


    uint8_t x = CX + round((double)(NEEDLE_LENGTH)*dx);
    uint8_t y = CY + round((double)(NEEDLE_LENGTH)*dy);
    if(x == needle_pos.x && y == needle_pos.y) {
        return 0;
    }
    needle_pos.x = x;
    needle_pos.y = y;

    return 1;
}

void compass_grid_draw_needle(){
    int cx = (int)CX;
    int cy = (int)CY;

    if(needle_bearing < M_PI / 2) {
        draw_line(cx, cy - 1, needle_pos.x, needle_pos.y - 1);
    }
    else if(needle_bearing < M_PI) {
        draw_line(cx - 1, cy - 1, needle_pos.x - 1, needle_pos.y - 1);

    }
    else if(needle_bearing < 3*M_PI/2) {
        draw_line(cx - 1, cy, needle_pos.x - 1, needle_pos.y);
    }
    else{
        draw_line(cx, cy, needle_pos.x, needle_pos.y);
    }
}
