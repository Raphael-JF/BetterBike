#ifndef LCD_CORE_H
#define LCD_CORE_H

#include <LiquidCrystal_I2C.h>

enum view_idx {
    CALIBRATION_VIEW = 0,
    GPS_VIEW = 1,
    NUM_VIEWS = 2
};

extern enum view_idx active_view_idx;


extern LiquidCrystal_I2C lcd;


#endif // LCD_CORE_H