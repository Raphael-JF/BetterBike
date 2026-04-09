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
    bool compass_needs_respring = false;
    const unsigned long now = millis();
    static unsigned long last_compass_sim_update = 0;
    const unsigned long compass_sim_period_ms = 100;

    if (now - last_compass_sim_update >= compass_sim_period_ms) {
        unsigned long elapsed = now - last_compass_sim_update;
        if (last_compass_sim_update == 0) {
            elapsed = compass_sim_period_ms;
        }
        last_compass_sim_update = now;

        bearing_to_display += 1.0 * ((double)elapsed / 1000.0); // pour test d'affichage de la boussole, à remplacer par le calcul du bearing GPS réel
        if (bearing_to_display >= 2 * M_PI) {
            bearing_to_display = fmod(bearing_to_display, 2 * M_PI);
        }

        if (calculate_compass_grid()) {
            compass_needs_respring = true;
        }
    }
    

    
    // 1. Lire les données GPS
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    const bool gps_time_fresh = gps.time.isValid() && gps.time.isUpdated() && gps.time.age() < 2000;
    const bool gps_date_fresh = gps.date.isValid() && gps.date.isUpdated() && gps.date.age() < 2000;
    const bool gps_fix_active = gps.location.isValid() && gps.location.age() < 2000;


    // 2. Utiliser les données seulement si elles sont valides et fraîches 

    if (gps_date_fresh) {
        utc_day = gps.date.day();
        utc_month = gps.date.month();
        utc_year = gps.date.year();
    }
    if (gps_fix_active) {
        update_current_position();    
        
        if (gps_time_fresh) {
            last_got_gps_time.hours = gps.time.hour();
            last_got_gps_time.minutes = gps.time.minute();
        }
    }


    if (update_time()) {
        lcd_respring_time();
    }

    // 3. Mettre à jour l'affichage de la boussole
    compass_needs_respring |= update_compass_frame_blinking();
    compass_needs_respring |= update_gps_timeout_status();

    if (compass_needs_respring) {
        lcd_respring_compass();
    }

     
    while (ble.available()) {
        char c = ble.read();
        Serial.write(c); // debug brut
    }
        // ici tu parses lat/lon

    delay(5);
}