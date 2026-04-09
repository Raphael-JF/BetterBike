#include <math.h>
#include <stdint.h>
#include <string.h>

#include <Wire.h>
#include "Arduino.h"


#include "gps/gps_core.h"
#include "time/time_manager.h"
#include "lcd/lcd_core.h"
#include "bluetooth/bluetooth.h"
#include "time/blinking.h"

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


    // initialiser la connexion Bluetooth
    ble.begin("VeloGPS");

    // initialiser le magnétomètre
    if (!magnetometer.begin(Wire, QMC5883P_SLAVE_ADDRESS)) {
        while (1) {
            Serial.println("Failed to find QMC5883P - check your wiring!");
            delay(1000);
        }
    }
    init_magnetometer();
    

    // afficher l'heure et le statut GPS par défaut
    lcd.setCursor(0, 0);
    lcd.print("--:--  GPS");

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
    clear_inner_compass();
    if(calculate_compass_grid()){
        lcd_respring_compass();
    }

    // afficher "Compass" à côté de l'icône de la boussole
    lcd_respring_gps_status();



}



void loop() {

    MagnetometerData data;

    if (magnetometer.readData(data)) {
        Serial.print(" Heading (deg): ");
        Serial.print(data.heading_degrees, 2);
        Serial.println("°");
    }


        
    bool compass_needs_respring = false;
    

    
    // 1. Lire les données GPS
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    const bool gps_time_fresh = gps.time.isValid() && gps.time.isUpdated() && gps.time.age() < 2000;
    const bool gps_date_fresh = gps.date.isValid() && gps.date.isUpdated() && gps.date.age() < 2000;
    const bool gps_fix_active = gps.location.isValid() && gps.location.isUpdated() && gps.location.age() < 2000;


    // 2. Utiliser les données seulement si elles sont valides et fraîches 

    if (gps_date_fresh) {
        utc_day = gps.date.day();
        utc_month = gps.date.month();
        utc_year = gps.date.year();
    }



    if (gps_fix_active) {
        update_current_position();
        if (update_bearing_to_waypoint() && calculate_compass_grid()) {
            compass_needs_respring = true;
        }
        if (gps_time_fresh) {
            last_got_gps_time.hours = gps.time.hour();
            last_got_gps_time.minutes = gps.time.minute();
        }
    }


    if (update_time()) {
        lcd_respring_time();
    }

    if (bluetooth_update_waypoint_from_stream()) {
        if (gps_fix_active && update_bearing_to_waypoint() && calculate_compass_grid()) {
            compass_needs_respring = true;
        }
    }

    // 3. Mettre à jour l'affichage de la boussole
    compass_needs_respring |= update_compass_frame_blinking();
    compass_needs_respring |= update_gps_timeout_status();

    if (compass_needs_respring) {
        lcd_respring_compass();
    }
    

    delay(5);
}