#include "NEW.h"



struct component NEW = {
    NEW_component_on_enter,
    NEW_update,
    create_flag_manager(NUM_NEW_FLAGS)
};


void NEW_component_on_enter() {
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

void NEW_update() {
    struct flag_manager* flags = NEW.flags;

    // if a flag is set..
    if(is_flag_set(flags, NEW_FLAG_1)){
        // do something
    }

    clear_all_flags(flags);
}