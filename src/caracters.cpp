#include <stdint.h>

#include "caracters.h"
#include "lcd_sim.h"

extern int x_start, y_start;
extern uint8_t grid[H][W];

uint8_t dash_car[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b00000,
    0b00000,
    0b00000
};

uint8_t pipe_car[8] = {
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100
};

uint8_t zero_car[8] = {
    0b01110,
    0b10001,
    0b10011,
    0b10101,
    0b11001,
    0b10001,
    0b10001,
    0b01110
};

uint8_t one_car[8] = {
    0b00100,
    0b01100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b01110
};

uint8_t two_car[8] = {
    0b01110,
    0b10001,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b10000,
    0b11111
};

uint8_t three_car[8] = {
    0b01110,
    0b10001,
    0b00001,
    0b00110,
    0b00001,
    0b00001,
    0b10001,
    0b01110
};

uint8_t four_car[8] = {
    0b00010,
    0b00110,
    0b01010,
    0b10010,
    0b11111,
    0b00010,
    0b00010,
    0b00010
};

uint8_t five_car[8] = {
    0b11111,
    0b10000,
    0b11110,
    0b00001,
    0b00001,
    0b00001,
    0b10001,
    0b01110
};

uint8_t six_car[8] = {
    0b00110,
    0b01000,
    0b10000,
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b01110
};

uint8_t seven_car[8] = {
    0b11111,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b01000,
    0b01000,
    0b01000
};

uint8_t eight_car[8] = {
    0b01110,
    0b10001,
    0b10001,
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b01110
};

uint8_t nine_car[8] = {
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b01111,
    0b00001,
    0b00010,
    0b01100
};

uint8_t *digits_car[10] = {
    zero_car, one_car, two_car, three_car, four_car,
    five_car, six_car, seven_car, eight_car, nine_car
};

