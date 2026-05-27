#include "display/views/cal_view.h"
#include "magnetometer/calibrate.h"
#include "magnetometer/magnetometer.h"
#include "utils/flag_manager.h"
/* 
XXXXXX XY XXXXXX
XXXXXX xy XXXXXX
*/ 

struct text* text_xmin = &Text1;
struct text* text_xmax = &Text2;
struct text* text_ymin = &Text3;
struct text* text_ymax = &Text4;

struct flag_manager* cal_view_flags = create_flag_manager(NUM_CAL_VIEW_FLAGS);


void enter_cal_view() {
    lcd.clear();
    lcd.setCursor(7, 0);
    lcd.print("XY");
    lcd.setCursor(7, 1);
    lcd.print("xy");

    change_text_message(text_xmin, (char *)"------");
    change_text_position(text_xmin, 0, 1);

    change_text_message(text_xmax, (char *)"------");
    change_text_position(text_xmax, 0, 0);

    change_text_message(text_ymin, (char *)"------");
    change_text_position(text_ymin, 10, 1);

    change_text_message(text_ymax, (char *)"------");
    change_text_position(text_ymax, 10, 0);

    Serial.println("Entering calibration view");

    text_update(text_xmin);
    text_update(text_xmax);
    text_update(text_ymin);
    text_update(text_ymax);
    active_view_idx = CALIBRATION_VIEW;


    // reset the min and max values for the magnetometer calibration
    magnetometer_calibrate_reset();
    Serial.println("Calibration values reset, start rotating the sensor to acquire new values...");
}


void update_cal_view(){
    if (is_flag_set(cal_view_flags, CAL_CHANGED_MAGNETOMETER_RAW_DATA)) {
        if(magnetometer_calibrate_aquire()){
             set_flag(cal_view_flags, CAL_CHANGED_MAGNETOMETER_BOUNDARIES);
        }
    }
    
    if (is_flag_set(cal_view_flags, CAL_CHANGED_MAGNETOMETER_BOUNDARIES)) {
        char xmin_message[7];
        char xmax_message[7];
        char ymin_message[7];
        char ymax_message[7];
        snprintf(xmin_message, 7, "%-6d", x_min);
        snprintf(ymin_message, 7, "%6d", y_min);
        snprintf(xmax_message, 7, "%-6d", x_max);
        snprintf(ymax_message, 7, "%6d", y_max);
        change_text_message(text_xmin, xmin_message);
        change_text_message(text_xmax, xmax_message);
        change_text_message(text_ymin, ymin_message);
        change_text_message(text_ymax, ymax_message);
    }
    text_update(text_xmin);
    text_update(text_xmax);
    text_update(text_ymin);
    text_update(text_ymax);
    clear_all_flags(cal_view_flags);
}
