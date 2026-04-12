#ifndef COMPASS_H
#define COMPASS_H

#define W_gps 15
#define H_gps 16

#define CX W_gps / 2.0
#define CY H_gps / 2.0

#define ARROW_LENGTH W_gps < H_gps ? W_gps/2 : H_gps/2

#include "display/lcd_core.h"
#include "utils/blinking.h"

struct compass_pos {
    uint8_t x;
    uint8_t y;
};


enum compass_flags : uint8_t {
    CHANGED_MAGNETOMETER_BEARING = 0,
    CHANGED_CURRENT_POSITION = 1,
    CHANGED_WAYPOINT_POSITION = 2, 
    CHANGED_WAYPOINT_BEARING = 3,
    CHANGED_BEARING_TO_DISPLAY = 4,
    CHANGED_COMPASS_GRID = 5,
    DO_HIGHLIGHT_FRAME = 6,
    DO_UNHIGHLIGHT_FRAME = 7,
    NUM_COMPASS_FLAGS = 8
};

enum gps_timeout_status_transition {
    NO_TRANSITION = 0,
    OK_TO_OLD = 1,
    OLD_TO_OK = 2,
    OLD_TO_INVALID = 3,
    INVALID_TO_OK = 4
};

extern struct compass_pos compass_pos;
extern struct bin_matrix* compass_grid;
extern double bearing_to_display;
extern blinking compass_frame_blinking;

extern struct component Compass;


/**
 * @brief Update GPS timeout state from elapsed time since last GPS sync.
 * @return Timeout state transition that occurred, or 0 if no transition happened.
 */
enum gps_timeout_status_transition update_gps_timeout_status();

/**
 * @brief Recompute the compass needle in `compass_grid` when heading changed enough.
 * @return Non-zero if the grid changed and must be redrawn, 0 otherwise.
 */
uint8_t calculate_compass_grid();

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




#endif // COMPASS_H
