#include "display/components/texts.h"

struct text Text1 = {
    0,
    0,
    "                ", // 16 spaces to clear the line when printed
    {
        nullptr,
        nullptr,
        create_flag_manager(NUM_TEXT_FLAGS)
    }
};

struct text Text2 = {
    0,
    0,
    "                ", // 16 spaces to clear the line when printed
    {
        nullptr,
        nullptr,
        create_flag_manager(NUM_TEXT_FLAGS)
    }
};

struct text Text3 = {
    0,
    0,
    "                ", // 16 spaces to clear the line when printed
    {
        nullptr,
        nullptr,
        create_flag_manager(NUM_TEXT_FLAGS)
    }
};

void display_refresh_text(struct text *t) {
    lcd.setCursor(t->col, t->row);
    lcd.print(t->message);
}

void change_text_message(struct text *t, char* new_message) {
    uint8_t i = 0;
    uint8_t changed = 0;
    while (i < 17 && new_message[i] != '\0') {
        if (t->message[i] != new_message[i]) {
            t->message[i] = new_message[i];
            set_flag(t->component.flags, CHANGED_TEXT);
        }
        i++;
    }
    t->message[i] = '\0'; // Ensure null termination
    i++;
    while(i < 17) {
        if (t->message[i] != ' ') {
            t->message[i] = ' ';
                        set_flag(t->component.flags, CHANGED_TEXT);

        }
        i++;
    }
}

void change_text_position(struct text *t, uint8_t col, uint8_t row) {
    if (t->col != col || t->row != row) {
        t->col = col;
        t->row = row;
        set_flag(t->component.flags, CHANGED_TEXT);
    }
}