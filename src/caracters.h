#ifndef CARACTERS_H
#define CARACTERS_H

#include <stdint.h>

extern uint8_t dash_car[8];
extern uint8_t pipe_car[8];
extern uint8_t zero_car[8];
extern uint8_t one_car[8];
extern uint8_t two_car[8];
extern uint8_t three_car[8];
extern uint8_t four_car[8];
extern uint8_t five_car[8];
extern uint8_t six_car[8];
extern uint8_t seven_car[8];
extern uint8_t eight_car[8];
extern uint8_t nine_car[8];
extern uint8_t *digits_car[10];



void draw_caracter(int x, int y, uint8_t char_data[8]);
void extract_char(int x, int y, uint8_t out[8]);

#endif // CARACTERS_H