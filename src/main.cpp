#include <math.h>
#include <stdint.h>
#include <string.h>

#include <Wire.h>
#include "Arduino.h"


#include "gps/gps_core.h"
#include "display/lcd_core.h"
#include "display/compass.h"
#include "display/clock.h"
#include "bluetooth/bluetooth.h"

#include "magnetometer/magnetometer.h"

#include "utils/component.h"






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


    // initialiser l'advertising Bluetooth
    ble.begin("VeloGPS");

    // initialiser le magnétomètre
    if (!magnetometer.begin(Wire, QMC5883P_SLAVE_ADDRESS)) {
        while (1) {
            Serial.println("Failed to find QMC5883P - check your wiring!");
            delay(1000);
        }
    }
    init_magnetometer();
    magnetometer.setOffset(-900,-360,0); // à ajuster pour compenser les perturbations magnétiques locales (aimantation résiduelle du vélo, etc.)
    

    // charger la vue par défaut (inclut l'initialisation des composants, dont la boussole)
    lcd_load_view(GPS_VIEW);
}





void loop() {

    switch (current_view)
    {
case CALIBRATION_VIEW:
    
    









case COMPASS_VIEW:





    // if the minutes changed or if the new GPS time data made them change
    if(update_time()){
        warn_component(Clock, CHANGED_CURRENT_TIME);
    }




    break;

case GPS_VIEW:
    // if new GPS data is available
    if(read_gps_data()){
        warn_component(Compass, CHANGED_CURRENT_POSITION);
    }

    // if a new waypoint is sent by Bluetooth
    if (read_bluetooth_data()) {
        warn_component(Compass, CHANGED_WAYPOINT_POSITION);
    }

    // if the magnetometer bearing has changed
    if(update_magnetometer_bearing()){
        warn_component(Compass, CHANGED_MAGNETOMETER_BEARING);
    }

    // if the Compass frame should blink
    switch (blinking_update(compass_frame_blinking)) {
        case BLINKING_STATE_ON:
            warn_component(Compass, DO_HIGHLIGHT_FRAME);
            break;
        case BLINKING_STATE_OFF:
            warn_component(Compass, DO_UNHIGHLIGHT_FRAME);
            break;
        default:
            break;
    }

    // if the GPS timeout status has changed
    switch (update_gps_timeout_status()) {
        case OK_TO_OLD:
            warn_component(Compass, DO_HIGHLIGHT_FRAME);
            break;
        case OLD_TO_OK:
            blinking_stop(compass_frame_blinking);
            warn_component(Compass, DO_HIGHLIGHT_FRAME);
            break;
        case OLD_TO_INVALID:
            blinking_stop(compass_frame_blinking);
            warn_component(Compass, DO_UNHIGHLIGHT_FRAME);
            break;
        case INVALID_TO_OK:
            warn_component(Compass, DO_HIGHLIGHT_FRAME);
            break;
        default:
            break;
    }

    // if the minutes changed or if the new GPS time data made them change
    if(update_time()){
        warn_component(Clock, CHANGED_CURRENT_TIME);
    }


    break;
        
    
    


default:
    break;
}
    


    


    lcd_update_current_view();

    delay(5);
}