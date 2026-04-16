#ifndef NAV_COMPASS_H
#define NAV_COMPASS_H




#include "display/compass/compass_core.h"
#include "display/lcd_core.h"

#include "gps/gps_core.h"

#include "utils/bin_matrix.h"
#include "utils/blinking.h"
#include "utils/flag_manager.h"
#include "utils/component.h"

#include "magnetometer/magnetometer.h"




enum nav_compass_flags : uint8_t {
    NAV_CHANGED_MAGNETOMETER_RAW_DATA = 0,
    NAV_CHANGED_MAGNETOMETER_BEARING = 1,
    NAV_CHANGED_CURRENT_POSITION = 2,
    NAV_CHANGED_WAYPOINT_POSITION = 3, 
    NAV_CHANGED_WAYPOINT_BEARING = 4,
    NAV_CHANGED_needle_bearing = 5,
    NAV_CHANGED_COMPASS_GRID = 6,
    NAV_DO_HIGHLIGHT_FRAME = 7,
    NAV_DO_UNHIGHLIGHT_FRAME = 8,
    NAV_CHANGED_NEEDLE_POSITION = 9,
    NUM_NAV_COMPASS_FLAGS = 10
};

enum gps_timeout_status_transition : uint8_t {
    NO_TRANSITION = 0,
    OK_TO_OLD = 1,
    OLD_TO_OK = 2,
    OLD_TO_INVALID = 3,
    INVALID_TO_OK = 4
};

extern blinking gps_compass_frame_blinking;

extern struct component Nav_compass;


void nav_compass_component_on_enter();
void nav_compass_update();



/**
 * @brief Update GPS timeout state from elapsed time since last GPS sync.
 * @return Timeout state transition that occurred, or 0 if no transition happened.
 */
enum gps_timeout_status_transition update_gps_timeout_status();

/**
 * @brief Recompute the compass needle in `compass_grid` when heading changed enough.
 * @return Non-zero if the grid changed and must be redrawn, 0 otherwise.
 */
void compass_grid_draw_needle();


/*
    Met à jour needle_bearing en radians pour pointer du current_position vers waypoint_position, en utilisant magnetometer_bearing pour calculer l'angle de l'aiguille de la boussole. Normalise l'angle entre 0 et 2*PI. Retourne 1 si needle_bearing a été changé, 0 sinon
*/
uint8_t nav_update_needle_bearing();



#endif // NAV_COMPASS_H
