#ifndef LCD_CORE__H
#define LCD_CORE__H

#include <LiquidCrystal_I2C.h>



#define NO_VIEW 0
#define GPS_STATUS_VIEW 1
#define CALIBRATION_VIEW 2
#define COMPASS_VIEW 3



extern LiquidCrystal_I2C lcd;



/*
    Affiche la vue souhaitée. Une vue comprend deux zones d'affichage :
        - un affichage à droite de l'heure (colonnes 6 à 11 : 6 caractères) 
        - un autre affichage en bas à gauche (colonnes 0 à 11 : 12 caractères)
*/
void lcd_load_view(uint8_t view);



#endif // LCD_CORE__H
