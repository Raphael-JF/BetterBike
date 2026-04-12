#ifndef COMPASS_H
#define COMPASS_H

#define W_gps 15
#define H_gps 16

#define CX W_gps / 2.0
#define CY H_gps / 2.0

#define ARROW_LENGTH W_gps < H_gps ? W_gps/2 : H_gps/2


#include "utils/blinking.h"
struct compass_pos {
    uint8_t x;
    uint8_t y;
};



extern struct compass_pos compass_pos;
extern struct bin_matrix* compass_grid;
extern double bearing_to_display;
extern blinking compass_frame_blinking;


/* 
    Met à jour l'affichage de la boussole en utilisant la grille compass_grid. Doit être appelé après toute modification de cette grille pour que les changements soient visibles à l'écran.
*/
void display_refresh_compass();

/* 
    Met à jour l'affichage de l'heure.
*/
void display_refresh_time();

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
    Met à jour l'état du clignotement du cadre de la boussole en fonction du statut GPS.
*/
uint8_t update_compass_frame_blinking();


#endif // COMPASS_H
