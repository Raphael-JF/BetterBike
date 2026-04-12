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
    lcd_load_view(CALIBRATION_VIEW);
}


uint8_t gps_time_fresh;
uint8_t gps_date_fresh;
uint8_t gps_fix_fresh;


void loop() {


    

    
    // Lire les données GPS
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }
    
    gps_time_fresh = gps.time.isValid() && gps.time.isUpdated() && gps.time.age() < 2000;
    gps_date_fresh = gps.date.isValid() && gps.date.isUpdated() && gps.date.age() < 2000;
    gps_fix_fresh = gps.location.isValid() && gps.location.isUpdated() && gps.location.age() < 2000;



    //  Utiliser les données seulement si elles sont valides et fraîches 
    if (gps_date_fresh) {
        utc_day = gps.date.day();
        utc_month = gps.date.month();
        utc_year = gps.date.year();
    }



    if (gps_fix_fresh) {
        update_current_position();
        warn_component(Compass, CHANGED_CURRENT_POSITION);
        if (gps_time_fresh) {
            last_got_gps_time.hours = gps.time.hour();
            last_got_gps_time.minutes = gps.time.minute();
        }
    }

    if(update_magnetometer_bearing()){
        warn_component(Compass, CHANGED_MAGNETOMETER_BEARING);
    }

    if (bluetooth_update_waypoint_from_stream()) {
        warn_component(Compass, CHANGED_WAYPOINT_POSITION);
    }

    if(update_compass_frame_blinking() || update_gps_timeout_status()){
        warn_component(Compass, CHANGED_COMPASS_GRID);
    }


    if(update_time()){
        warn_component(Time, CHANGED_CURRENT_TIME);
    }


    lcd_update_current_view();

    delay(5);
}