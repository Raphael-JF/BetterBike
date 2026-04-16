#ifndef TEXTS_H
#define TEXTS_H

#include <stdint.h>
#include <stdlib.h>

#include "display/lcd_core.h"

#include "utils/component.h"

enum text_flags : uint8_t {
    CHANGED_TEXT = 0,
    NUM_TEXT_FLAGS = 1,
};

struct text {
    uint8_t col;
    uint8_t row;
    char message[17];
    struct component component;
};



extern struct text Text1;
extern struct text Text2;
extern struct text Text3;


/**
 * @brief Push the text `t` to the LCD at the position defined by its attributes `col` and `row`.
 * The text is expected to be at most 16 characters long (including the null terminator).
 */
void display_refresh_text(struct text *t);

/**
 * @brief Update the text `t` if needed, for example by changing its `message` attribute, and then refresh it on the LCD. 
 * The component will warn itself if it was changed and needs to be refreshed.
 */
void change_text_message(struct text *t, char* new_message) ;

/**
 * @brief Update the position of the text `t` on the LCD. The component will warn itself if it was changed and needs to be refreshed.
 */
void change_text_position(struct text *t, uint8_t col, uint8_t row);



#endif // TEXTS_H