#ifndef LCD_CORE__H
#define LCD_CORE__H

#include <LiquidCrystal_I2C.h>

#define W_gps 15
#define H_gps 16

#define CX W_gps / 2.0
#define CY H_gps / 2.0

#define ARROW_LENGTH W_gps < H_gps ? W_gps/2 : H_gps/2
struct compass_pos {
    uint8_t x;
    uint8_t y;
};

extern LiquidCrystal_I2C lcd;
extern struct compass_pos compass_pos;
extern struct bin_matrix* compass_grid;
extern double bearing_to_display;



/*
    Met à jour l'affichage du statut GPS (icône de satellite ou de croix) en fonction de la variable globale is_gps_active.
*/
void lcd_respring_gps_status();

/* 
    Met à jour l'affichage de la boussole en utilisant la grille compass_grid. Doit être appelé après toute modification de cette grille pour que les changements soient visibles à l'écran.
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
    Éteint les pixels de la grille compass_grid qui forment le cadre de la boussole. N'interfère pas avec ceux formant l'aiguille, ne nécessitant ainsi pas de recalcul de celle-ci pour faire clignoter le cadre.
*/
void unhighlight_compass_frame();

/* 
    Efface les pixels de la grille compass_grid qui forment le cadre de la boussole à l'exceptions de ceux formant le bord clignotant.
*/
void clear_inner_compass();

/*
    Dessine sur compass_grid l'aiguille, si besoin. Renvoie un non zéro si l'aiguille doit être redessinée (si le bearing a changé suffisamment pour que l'aiguille doive être redessinée), sinon 0.
*/
uint8_t calculate_compass_grid();


/*
    Dessine une ligne de Bresenham entre les points (x0, y0) et (x1, y1) sur compass_grid. Les points doivent être dans les limites de la grille.
*/
void draw_line(int x0, int y0, int x1, int y1);

#endif // LCD_CORE__H
