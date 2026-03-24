#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <Arduino.h>
#include "gps.h"
#include "lcd_sim.h"
#include "caracters.h"

uint8_t compass_grid[H][W];

void display_lcd() {
    // Clear terminal
    printf("\033[H");
    printf("\n");

    for (int cy = 0; cy < H/8; cy++) {      // lignes de caractères
        // Ligne horizontale du haut
        for (int cx = 0; cx < W/5; cx++) {
            printf("|-----");
            // printf("      ");
        }
        printf("|\n");

        for (int y = 0; y < 8; y++) {     // pixels par caractère
            for (int cx = 0; cx < W/5; cx++) { // colonnes de caractères
                printf("|");              // bord gauche du caractère
                for (int x = 0; x < 5; x++) {
                    int px = cx * 5 + x;
                    int py = cy * 8 + y;
                    printf(compass_grid[py][px] ? "*" : " ");
                }
            }
            printf("|\n");                 // bord droit + fin de ligne
        }
    }

    // Ligne horizontale finale
    for (int cx = 0; cx < W/5; cx++) {
        printf("|-----");
    }
    printf("|\n");
}

int main() {
    float angle = 0.0;

    draw_caracter(12, 0, pipe_car);
    draw_caracter(12, 1, pipe_car);

    while (1) {
        set_cursor(13, 0);
        update_compass(angle);
        // compass_grid[y_start + 0][x_start + 0] = 1;
        display_lcd();

        angle += 0.1; // incrémente pour rotation
        if (angle > 2*M_PI)
            angle -= 2*M_PI;

        delay(100); // 100 ms
    }

    return 0;
}