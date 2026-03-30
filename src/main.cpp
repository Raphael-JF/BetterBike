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

    Wire.begin(21, 22); // SDA, SCL
    gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // UART1

    // set up the lcd's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setRGB(127, 127, 127);

    ble.begin("VeloGPS");
    


    lcd.setCursor(0, 0);
    lcd.print("--:--  GPS");

    // logo bluetooth :
    uint8_t bluetooth_car[8] = {
        0b00110,
        0b10101,
        0b01101,
        0b00110,
        0b01101,
        0b10101,
        0b00110,
        0b00000
    };

    lcd.createChar(6, bluetooth_car);
    lcd_respring_gps_status();

    lcd.setCursor(13, 0);

    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.setCursor(13, 1);
    lcd.write(byte(3));
    lcd.write(byte(4));
    lcd.write(byte(5));

    lcd_respring_compass();


}


void loop() {

    update_all_blinkings();

    // bearing_to_display += 0.1; // pour test d'affichage de la boussole, à remplacer par le calcul du bearing GPS réel
    // if (bearing_to_display >= 2*M_PI) {
    //     bearing_to_display -= 2*M_PI;
    // }
    // clear_compass_frame();
    // lcd_respring_compass();
    Serial.println(last_gps_sync_millis);
    unsigned long current_millis = millis();
    switch (timeout_status) {
        case GPS_TIMEOUT_STATUS_OK:
            if (current_millis - last_gps_sync_millis > GPS_TIMEOUT_OLD){
                // "OK" -> "OLD", faire clignoter le cadre de la boussole pour indiquer que les données GPS sont anciennes
                timeout_status = GPS_TIMEOUT_STATUS_OLD;
                compass_frame_blinking = COMPASS_FRAME_BLINKING_INIT;
                blinkings_to_update[IDX_COMPASS_FRAME_BLINKING] = &compass_frame_blinking;

            }
            break;
        case GPS_TIMEOUT_STATUS_OLD:
            // faire clignoter le cadre de la boussole pour indiquer que les données GPS sont anciennes
            if (current_millis - last_gps_sync_millis < GPS_TIMEOUT_OLD) {
                // "OLD" -> "OK", arrêter de faire clignoter le cadre de la boussole
                timeout_status = GPS_TIMEOUT_STATUS_OK;
                blinkings_to_update[IDX_COMPASS_FRAME_BLINKING] = NULL;
                clear_compass_frame();  // enlever le contour de la boussole
                calculate_gps_grid();
                lcd_respring_compass();
            }
            if (current_millis - last_gps_sync_millis > GPS_TIMEOUT_INVALID) {
                // "OLD" -> "INVALID", activer le contour de la boussole pour indiquer que les données GPS sont invalides
                timeout_status = GPS_TIMEOUT_STATUS_INVALID;
                blinkings_to_update[IDX_COMPASS_FRAME_BLINKING] = NULL;
                highlight_compass_frame(); // ajouter le contour de la boussole pour indiquer que les données GPS sont invalides
                lcd_respring_compass();
            }
            break;
        case GPS_TIMEOUT_STATUS_INVALID:
            if (current_millis - last_gps_sync_millis < GPS_TIMEOUT_OLD) {
                // "INVALID" -> "OK"
                timeout_status = GPS_TIMEOUT_STATUS_OK;
                clear_compass_frame();  // enlever le contour de la boussole
                calculate_gps_grid();
                lcd_respring_compass();
            }
            break;
    }




    // 1. Lire les données GPS
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    const bool gps_time_fresh = gps.time.isValid() && gps.time.isUpdated() && gps.time.age() < 2000;
    const bool gps_date_fresh = gps.date.isValid() && gps.date.isUpdated() && gps.date.age() < 2000;
    const bool gps_fix_active = gps.location.isValid() && gps.location.age() < 2000;


    // 2. Utiliser les données seulement si elles sont valides et fraîches 
    if (gps_time_fresh) {
        last_got_gps_time.hours = gps.time.hour();
        last_got_gps_time.minutes = gps.time.minute();
        
    }
    if (gps_date_fresh) {
        utc_day = gps.date.day();
        utc_month = gps.date.month();
        utc_year = gps.date.year();
    }
    if (gps_fix_active) {
        Serial.print("act");
        update_current_position();
    }


    if (update_time()) {
        lcd_respring_time();
    }
     
    while (ble.available()) {
        char c = ble.read();
        Serial.write(c); // debug brut
    }
        // ici tu parses lat/lon


    delay(250);
}