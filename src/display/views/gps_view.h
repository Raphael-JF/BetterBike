#ifndef GPS_VIEW_H
#define GPS_VIEW_H

#include <stdint.h>


#include "magnetometer/magnetometer.h"
#include "gps/gps_core.h"
#include "display/components/compass.h"
#include "display/components/clock.h"
#include "display/lcd_core.h"
#include "utils/flag_manager.h"
#include "utils/blinking.h"

enum nav_compass_flags : uint8_t {
    GPS_CHANGED_MAGNETOMETER_RAW_DATA = 0,
    GPS_CHANGED_MAGNETOMETER_BEARING = 1,
    GPS_CHANGED_CURRENT_POSITION = 2,
    GPS_CHANGED_WAYPOINT_POSITION = 3, 
    GPS_CHANGED_WAYPOINT_BEARING = 4,
    GPS_CHANGED_NEEDLE_BEARING = 5,
    GPS_CHANGED_COMPASS_GRID = 6,
    GPS_DO_HIGHLIGHT_FRAME = 7,
    GPS_DO_UNHIGHLIGHT_FRAME = 8,
    GPS_CHANGED_NEEDLE_POSITION = 9,
    NUM_GPS_COMPASS_FLAGS = 10
};

enum gps_timeout_status_transition : uint8_t {
    NO_TRANSITION = 0,
    OK_TO_OLD = 1,
    OLD_TO_OK = 2,
    OLD_TO_INVALID = 3,
    INVALID_TO_OK = 4
};


extern blinking gps_compass_frame_blinking;
extern struct flag_manager* gps_compass_flags;

/**
 * @brief Initialize the GPS view, setting up components and initial display.
 */
void enter_gps_view();

/**
 * @brief Update the GPS view, refreshing components as needed.
 */
void update_gps_view();


/**
 * @brief Update the GPS compass display based on current flags and data.
 * Checks for changes in position, bearing, and magnetometer data to update the compass needle and frame.
 * Clears flags after processing.
 */
void gps_compass_update();

/**
 * @brief Update the compass needle bearing based on current waypoint and magnetometer bearings.
 * Normalizes the bearing to be between 0 and 2*PI.
 * @return 1 if the needle bearing was updated, 0 if it remained effectively unchanged.
 */
uint8_t gps_update_needle_bearing();


/**
 * @brief Update GPS timeout state from elapsed time since last GPS sync.
 * @return Timeout state transition that occurred, or 0 if no transition happened.
 */
enum gps_timeout_status_transition update_gps_timeout_status();


#endif // GPS_VIEW_H