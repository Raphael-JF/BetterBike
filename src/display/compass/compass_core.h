#ifndef COMPASS_CORE_H
#define COMPASS_CORE_H

#include <stdint.h>

#include "utils/binary_matrix.h"

#include "display/lcd_core.h"


#define W_gps 15
#define H_gps 16

#define CX W_gps / 2.0
#define CY H_gps / 2.0

struct compass_needle_pos {
    uint8_t x;
    uint8_t y;
};

extern struct compass_needle_pos compass_pos;
extern struct bin_matrix* compass_grid;
extern double bearing_to_display;

/**
 * @brief Push current `compass_grid` pixels to LCD custom characters.
 */
void display_refresh_compass();

/**
 * @brief Enable the blinking compass frame pixels in `compass_grid`.
 */
void highlight_compass_frame();

/**
 * @brief Disable the blinking compass frame pixels in `compass_grid`.
 */
void unhighlight_compass_frame();

/**
 * @brief Clear compass interior pixels while preserving the frame pixels.
 */
void clear_inner_compass();

/**
 * @brief Clear the whole compass grid, including frame pixels.
 */
void clear_whole_compass();



#endif // COMPASS_CORE_H