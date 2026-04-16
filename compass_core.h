#ifndef COMPASS_CORE_H
#define COMPASS_CORE_H

#include <stdint.h>

#include "utils/bin_matrix.h"

#include "display/lcd_core.h"


#define W_COMPASS 15
#define H_COMPASS 16

#define NEEDLE_LENGTH ((W_COMPASS < H_COMPASS) ? (W_COMPASS / 2.0) : (H_COMPASS / 2.0))


#define CX (W_COMPASS / 2.0)
#define CY (H_COMPASS / 2.0)

struct compass_needle_pos {
    uint8_t x;
    uint8_t y;
};


extern struct compass_needle_pos needle_pos;
extern struct bin_matrix* compass_grid;
extern double needle_bearing;





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
void clear_inner_compass_grid();

/**
 * @brief Clear the whole compass grid, including frame pixels.
 */
void clear_whole_compass();


/**
 * @brief Update the position of the compass needle based on the current bearing.
 */
uint8_t update_needle_position() ;

/**
 * @brief Draw a line on the compass grid from the center to the needle position.
 * Uses Bresenham's line algorithm.
 * @param x0 Starting point x coordinate (in pixels)
 * @param y0 Starting point y coordinate (in pixels)
 * @param x1 Ending point x coordinate (in pixels)
 * @param y1 Ending point y coordinate (in pixels)
 */
void compass_grid_draw_needle();



#endif // COMPASS_CORE_H