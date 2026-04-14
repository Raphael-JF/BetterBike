#ifndef LCD_CORE__H
#define LCD_CORE__H

#include <stdint.h>
#include <LiquidCrystal_I2C.h>

#include "display/lcd_core.h"
#include "display/compass/nav_compass.h"
#include "display/compass/cal_compass.h"
#include "display/clock.h"
#include "gps/gps_core.h"

struct view {
    uint8_t num_components;
    struct component* components;
};

enum view_idx : uint8_t{
    NO_VIEW = 0,
    GPS_VIEW = 1,
    CALIBRATION_VIEW = 2,
    COMPASS_VIEW = 3
};
extern LiquidCrystal_I2C lcd;
extern enum view_idx current_view;
extern struct view* active_view;

extern struct view calibration_view;
extern struct view compass_view;
extern struct view empty_view;

/*
    Returns the corresponding struct view.
*/
struct view* map_view(enum view_idx view);

/*
    Affiche la vue souhaitée. Une vue comprend deux zones d'affichage :
        - un affichage à droite de l'heure (colonnes 6 à 11 : 6 caractères) 
        - un autre affichage en bas à gauche (colonnes 0 à 11 : 12 caractères)
*/
void lcd_load_view(enum view_idx view);

/*
    Met à jour les composants de la vue courante.
*/
void lcd_update_current_view(void);



#endif // LCD_CORE__H
