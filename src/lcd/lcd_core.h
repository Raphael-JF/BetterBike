#ifndef LCD_CORE__H
#define LCD_CORE__H

#include <LiquidCrystal_I2C.h>

#define W_gps 15 // 
#define H_gps 16 // 

extern LiquidCrystal_I2C lcd;
extern struct bin_matrix* compass_grid;
extern double bearing_to_display;


/*
    Met à jour l'affichage du statut GPS (icône de satellite ou de croix) en fonction de la variable globale is_gps_active.
*/
void lcd_respring_gps_status();

/* 
    Met à jour l'affichage de la boussole.
*/
void lcd_respring_compass();

/* 
    Met à jour l'affichage de l'heure.
*/
void lcd_respring_time();

/* 
    Allume les pixels de la grille compass_grid qui forment le cadre de la boussole. N'interfère pas avec ceux formant l'aiguille, ne nécessitant ainsi pas de recalcul de celle-ci pour faire clignoter le cadre.
*/
void highlight_compass_frame();

/* 
    Efface les pixels de la grille compass_grid qui forment le cadre de la boussole.
*/
void clear_compass_frame();

/*
    Dessine sur la grille compass_grid l'aiguille de la boussole. Ne réinitilise pas la grille avant de dessiner.
*/
void calculate_gps_grid();


/*
    Dessine une ligne de Bresenham entre les points (x0, y0) et (x1, y1) sur compass_grid. Les points doivent être dans les limites de la grille.
*/
void draw_line(int x0, int y0, int x1, int y1);

#endif // LCD_CORE__H
