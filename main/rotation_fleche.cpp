#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "rotation_fleche.h"


uint8_t grid[H][W]; // [y][x]


void dessine_fleche(float angle) {

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            grid[y][x] = 0;
        }
    }
    float dx = cos(angle);
    float dy = -sin(angle);

    for (int i = 0; i < 8; i++) { // longueur
        int x = (int)(cx + dx * i);
        int y = (int)(cy + dy * i);

        if (x >= 0 && x < W && y >= 0 && y < H)
            grid[y][x] = 1;
    }

    // pointe de flèche
    int tipx = (int)(cx + dx * 8);
    int tipy = (int)(cy + dy * 8);

    for (int i = -1; i <= 1; i++) {
        int px = tipx - dx + i * (-dy);
        int py = tipy - dy + i * (dx);

        if (px >= 0 && px < W && py >= 0 && py < H)
            grid[py][px] = 1;
    }
}

void extract_char(int x0, int y0, uint8_t out[8]) {
    for (int y = 0; y < 8; y++) {
        uint8_t ligne = 0;

        for (int x = 0; x < 5; x++) {
            if (grid[y0 + y][x0 + x]) {
                ligne |= (1 << (4 - x));
            }
        }

        out[y] = ligne;
    }
}

// int main(){
//     for (float angle = 0; angle < 2 * M_PI; angle += M_PI / 4) {
//         update_arrow(angle);
//     }

//     return 0;
// }