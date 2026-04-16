#include <math.h>
#include <stdint.h>
#include <string.h>

#include <Wire.h>
#include "Arduino.h"


#include "gps/gps_core.h"
#include "display/lcd_core.h"
// #include "display/compass/compass_core.h"
// #include "display/compass/nav_compass.h"
#include "display/views/gps_view.h"
#include "display/views/cal_view.h"
// #include "display/compass/cal_compass.h"
#include "display/components/clock.h"
#include "bluetooth/bluetooth.h"

#include "magnetometer/magnetometer.h"

#include "utils/component.h"
#include "utils/blinking.h"





void setup() {
    Serial.begin(115200);

    // initialiser la connexion I2C pour l'écran LCD 
    Wire.begin(21, 22); // SDA, SCL

    // initaliser la connexion série pour le GPS
    gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // UART1

    // initialiser l'écran LCD, attendre que le PCF8574 soit prêt
    while (lcd.begin(16, 2, LCD_5x8DOTS) != 1) {
        Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
        delay(5000);   
    }
    // lcd.noBacklight();

    // initialiser l'advertising Bluetooth
    ble.begin("VeloGPS");

    // initialiser le magnétomètre
    if (!init_magnetometer()) {
        while (1) {
            Serial.println("Failed to find QMC5883P - check your wiring!");
            delay(1000);
        }
    }
    

    // charger la vue par défaut (inclut l'initialisation des composants, dont la boussole)
    enter_cal_view();
}





void loop() {

    switch (active_view_idx)
    {
case CALIBRATION_VIEW:   
    if (read_magnetometer_data()) {
        warn_component(Compass, CAL_CHANGED_MAGNETOMETER_RAW_DATA);
    }

    if(num_calibration_points_done == NUM_COMPASS_PORTIONS * NUM_CALIBRATION_POINTS_PER_PORTION){
        // if the calibration is done, we can switch to the GPS view
        enter_gps_view();
        return;
    }

    update_cal_view();

    break;

case GPS_VIEW: 
    // if the minutes changed or if the new GPS time data made them change
    if(update_time()){
        warn_component(Clock, CHANGED_CURRENT_TIME);
    }

    // if new GPS data is available
    if(read_gps_data()){
        warn_component(Compass, GPS_CHANGED_CURRENT_POSITION);
    }

    // if a communication is received via Bluetooth
    switch (read_bluetooth_data()) {
        case BLUETOOTH_EVENT_WAYPOINT_RECEIVED:
            warn_component(Compass, GPS_CHANGED_WAYPOINT_POSITION);
            break;
        case BLUETOOTH_EVENT_CALIBRATE_RECEIVED:
            enter_cal_view();
            return;
        default:
            break;
    }
    
    // if the magnetometer raw data has changed
    if(read_magnetometer_data()){
        warn_component(Compass, GPS_CHANGED_MAGNETOMETER_RAW_DATA);
    }

    // if the Compass frame should blink
    switch (blinking_update(&gps_compass_frame_blinking)) {
        case BLINKING_SWITCHED_ON:
            warn_component(Compass, GPS_DO_HIGHLIGHT_FRAME);
            break;
        case BLINKING_SWITCHED_OFF:
            warn_component(Compass, GPS_DO_UNHIGHLIGHT_FRAME);
            break;
        default:
            break;
    }

    // if the GPS timeout status has changed
    switch (update_gps_timeout_status()) {
        case OK_TO_OLD:
            blinking_start(&gps_compass_frame_blinking);
            warn_component(Compass, GPS_DO_UNHIGHLIGHT_FRAME);
            break;
        case OLD_TO_OK:
            blinking_stop(&gps_compass_frame_blinking);
            warn_component(Compass, GPS_DO_HIGHLIGHT_FRAME);
            break;
        case OLD_TO_INVALID:
            blinking_stop(&gps_compass_frame_blinking);
            warn_component(Compass, GPS_DO_UNHIGHLIGHT_FRAME);
            break;
        case INVALID_TO_OK:
            warn_component(Compass, GPS_DO_HIGHLIGHT_FRAME);
            break;
        default:
            break;
    }
    update_gps_view();
    break;

        
    
    


default:
    break;
}


    delay(5);
}