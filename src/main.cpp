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

#include "magnetometer/qmc5883p_raw.h"
#include "utils/component.h"
#include "utils/blinking.h"

static unsigned long last_comp_notify_ms = 0;

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

    // init BLE (NimBLE-Arduino custom service)
    bluetooth_init("VeloGPS");

    // initialiser le magnétomètre
    if (!init_magnetometer()) {
        while (1) {
            Serial.println("Failed to find QMC5883P - check your wiring!");
            delay(1000);
        }
    }

    // Enable raw magnetometer streaming for plotting.
    // You can disable this later if you don't want continuous serial output.
    //magnetometer_stream_raw_set_period_ms(20); // 50 Hz
    //magnetometer_stream_raw_enable(1);

    // charger la vue par défaut (inclut l'initialisation des composants, dont la boussole)
    enter_gps_view();
}

void loop() {
    // Always tick the raw stream (it rate-limits internally and prints only on new data).
    //magnetometer_stream_raw_tick();

    switch (active_view_idx) {
        case CALIBRATION_VIEW: {
            if (read_magnetometer_data()) {
                set_flag(cal_view_flags, CAL_CHANGED_MAGNETOMETER_RAW_DATA);

                // Stream calibration points only when calibration UI mode is active (entered via BLE)
                // We don't have direct access to raw values here without touching read-only files,
                // so we only stream a compensator snapshot periodically. Calibration points should be
                // sent from wherever raw points are available; see note below.
            }

            // if a communication is received via Bluetooth
            switch (read_bluetooth_data()) {
                case BLE_ENTER_CAL:
                    enter_cal_view();
                    return;
                case BLE_ENTER_GPS:
                    enter_gps_view();
                    return;
                case BLE_SAVE_CAL:
                    magnetometer_calibrate_compute_offsets();
                    enter_gps_view();
                    return;
                default:
                    break;
            }

            unsigned long now = millis();
            if (now - last_comp_notify_ms >= 250) {
                last_comp_notify_ms = now;

                // Without editing read-only magnetometer headers, we cannot reliably read the
                // current compensator values here. For now, send placeholders (0,0).
                // Once you add the file that exposes the current magnetometer_compensator instance,
                // we’ll wire real values.
                Serial.println("Sending calibration data...");
                bluetooth_notify_calibration_point(raw_data.x, raw_data.y, raw_data.z);
                bluetooth_notify_compensator(magnetometer_compensator.x_offset, magnetometer_compensator.y_offset);
            }

            update_cal_view();

            break;
        }

        case GPS_VIEW: {
            // if the minutes changed or if the new GPS time data made them change
            if (update_time()) {
                warn_component(Clock, CHANGED_CURRENT_TIME);
            }

            // if new GPS data is available
            if (read_gps_data()) {
                warn_component(Compass, GPS_CHANGED_CURRENT_POSITION);
            }

            // if a communication is received via Bluetooth
            switch (read_bluetooth_data()) {
                case BLE_NEW_WAYPOINT:
                    warn_component(Compass, GPS_CHANGED_WAYPOINT_POSITION);
                    return;
                case BLE_ENTER_CAL:
                    enter_cal_view();
                    return;
                default:
                    break;
            }

            // if the magnetometer raw data has changed
            if (read_magnetometer_data()) {
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
        }

        default:
            break;
    }

    delay(5);
}
