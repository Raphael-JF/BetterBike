#include "display/text/text1.h"


char text1_text[17] = "                "; // 16 spaces, to clear the line when printed on the LCD

uint8_t text1_col = 0;
uint8_t text1_row = 0;

struct component Text1 = {
    text1_component_on_enter,
    text1_update,
    create_flag_manager(NUM_TEXT1_FLAGS)
};


void text1_component_on_enter() {
    // set the cursor to the text position and print the text
    lcd.setCursor(text1_col, text1_row);
    lcd.print(text1_text);
}

void text1_update() {
    struct flag_manager* flags = Text1.flags;

    // if a flag is set..
    if(is_flag_set(flags, TEXT1_HAS_CHANGED_TEXT)){
        lcd.setCursor(text1_col, text1_row);
        lcd.print(text1_text);
    }

    clear_all_flags(flags);
}