#ifndef NAV_COMPASS_H
#define NAV_COMPASS_H



#define ARROW_LENGTH W_gps < H_gps ? W_gps/2 : H_gps/2

#include "display/compass/compass_core.h"
#include "display/lcd_core.h"

#include "gps/gps_core.h"

#include "utils/binary_matrix.h"
#include "utils/blinking.h"
#include "utils/flag_manager.h"
#include "utils/component.h"

#include "magnetometer/magnetometer.h"




enum nav_compass_flags : uint8_t {
    CHANGED_MAGNETOMETER_BEARING = 0,
    CHANGED_CURRENT_POSITION = 1,
    CHANGED_WAYPOINT_POSITION = 2, 
    CHANGED_WAYPOINT_BEARING = 3,
    CHANGED_BEARING_TO_DISPLAY = 4,
    CHANGED_COMPASS_GRID = 5,
    DO_HIGHLIGHT_FRAME = 6,
    DO_UNHIGHLIGHT_FRAME = 7,
    NUM_NAV_COMPASS_FLAGS = 8
};

enum gps_timeout_status_transition : uint8_t {
    NO_TRANSITION = 0,
    OK_TO_OLD = 1,
    OLD_TO_OK = 2,
    OLD_TO_INVALID = 3,
    INVALID_TO_OK = 4
};

extern blinking compass_frame_blinking;

extern struct component Nav_compass;


void nav_compass_component_on_enter();
void nav_compass_component_update();



/**
 * @brief Update GPS timeout state from elapsed time since last GPS sync.
 * @return Timeout state transition that occurred, or 0 if no transition happened.
 */
enum gps_timeout_status_transition update_gps_timeout_status();

/**
 * @brief Recompute the compass needle in `compass_grid` when heading changed enough.
 * @return Non-zero if the grid changed and must be redrawn, 0 otherwise.
 */
uint8_t calculate_nav_compass_grid();




#endif // NAV_COMPASS_H
