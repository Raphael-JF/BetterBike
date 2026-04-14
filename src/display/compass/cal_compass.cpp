#include "cal_compass.h"



struct component Cal_compass = {
    cal_compass_component_on_enter,
    cal_compass_component_update,
    create_flag_manager(NUM_CAL_COMPASS_FLAGS)
};


void cal_compass_component_on_enter() {
    lcd.setCursor(13, 0);
    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.setCursor(13, 1);
    lcd.write(byte(3));
    lcd.write(byte(4));
    lcd.write(byte(5));

    clear_whole_compass();
    display_refresh_compass();
}

void cal_compass_component_update() {
    // No dynamic behavior for now, but we need to have this function to be able to use the same component structure as Nav_compass and potentially add dynamic behavior later if needed.
}