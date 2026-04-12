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
#include "utils/blinking.h"

#include "magnetometer/magnetometer.h"






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
    

    // afficher l'heure et le statut GPS par défaut
    lcd.setCursor(0, 0);
    lcd.print("--:--");

    // initialiser les caractères personnalisés pour l'icône de la boussole
    lcd.setCursor(13, 0);
    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.setCursor(13, 1);
    lcd.write(byte(3));
    lcd.write(byte(4));
    lcd.write(byte(5));

    // dessiner le cadre de la boussole
    unhighlight_compass_frame();
    clear_inner_compass();
    display_refresh_compass();

    // afficher "Compass" à côté de l'icône de la boussole
    lcd_load_view(CALIBRATION_VIEW);



}

uint8_t do_update_bearing_to_display = 0;
uint8_t do_update_magnetometer_bearing = 0;
uint8_t do_update_waypoint_bearing = 0;

uint8_t do_calculate_compass_grid = 0;
uint8_t do_respring_compass = 0;



void loop() {


    

    
    // Lire les données GPS
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }
    
    const uint8_t gps_time_fresh = gps.time.isValid() && gps.time.isUpdated() && gps.time.age() < 2000;
    const uint8_t gps_date_fresh = gps.date.isValid() && gps.date.isUpdated() && gps.date.age() < 2000;
    const uint8_t gps_fix_active = gps.location.isValid() && gps.location.isUpdated() && gps.location.age() < 2000;



    //  Utiliser les données seulement si elles sont valides et fraîches 
    if (gps_date_fresh) {
        utc_day = gps.date.day();
        utc_month = gps.date.month();
        utc_year = gps.date.year();
    }



    if (gps_fix_active) {
        update_current_position();
        do_update_waypoint_bearing = 1;
        if (gps_time_fresh) {
            last_got_gps_time.hours = gps.time.hour();
            last_got_gps_time.minutes = gps.time.minute();
        }
    }

    if (magnetometer.readData(magnetometer_data)) {
        do_update_magnetometer_bearing = 1;
    }

    if (update_time()) {
        display_refresh_time();
    }

    if (bluetooth_update_waypoint_from_stream()) {
        do_update_waypoint_bearing = 1;
    }
    
    do_respring_compass |= update_gps_timeout_status();
    do_respring_compass |= update_compass_frame_blinking();



    // First layer
    if (do_update_waypoint_bearing && update_waypoint_bearing()) {
        do_update_waypoint_bearing = 0;
        do_update_bearing_to_display = 1;
    }

    if (do_update_magnetometer_bearing && update_magnetometer_bearing()) {
        do_update_magnetometer_bearing = 0;
        do_update_bearing_to_display = 1;
    }

    // Second layer
    if (do_update_bearing_to_display && update_bearing_to_display()){
        do_update_bearing_to_display = 0;
        do_calculate_compass_grid = 1;
    }

    // Third layer
    if (do_calculate_compass_grid) {
        do_calculate_compass_grid = 0;
        calculate_compass_grid();
        do_respring_compass = 1;
    }

    // Fourth layer
    if (do_respring_compass) {
        do_respring_compass = 0;
        display_refresh_compass();
    }

    delay(5);
}