#include <stdint.h>
#include <stdio.h>

#include "display/lcd_core.h"
#include "display/clock.h"
#include "utils/time.h"

// Base temporelle actuellement affichée à l'écran (en UTC)
struct time_base current_time = {100, 100};

// Dernière base temporelle GPS reçue
struct time_base last_got_gps_time = {100, 100};

// Dernière base temporelle GPS prise en compte pour faire avancer l'heure avec millis(). Elle est en UTC.
struct time_base last_used_gps_time = {100, 100};

// Dernière base temporelle GPS prise en compte pour faire avancer l'heure avec millis(). Elle est en fuseau local français.
struct time_base last_used_gps_time_local = {100, 100};

// Valeur de millis() au moment où gps_base_hour et gps_base_minutes ont été synchronisées
unsigned long last_time_sync_millis = 0;


uint16_t utc_year = 2026;
uint8_t utc_month = 4;
uint8_t utc_day = 0;


void time_component_on_enter() {
    lcd.setCursor(0, 0);
    lcd.print("--:--");
}

void time_component_update() {
    flag_manager* flags = Time.flags;

    if (is_flag_set(flags, CHANGED_CURRENT_TIME)) {
        display_refresh_time();
    }
    clear_all_flags(flags);

}


struct component Time = {
    time_component_on_enter,
    time_component_update,
    create_flag_manager(NUM_CLOCK_FLAGS)
};


void display_refresh_time() {
    char time_str[6];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", current_time.hours, current_time.minutes);
    lcd.setCursor(0, 0);
    lcd.print(time_str);
}

int update_time() {
    if (last_got_gps_time.hours != 100 && last_got_gps_time.minutes != 100) {
        if (last_got_gps_time.minutes != last_used_gps_time.minutes || last_got_gps_time.hours != last_used_gps_time.hours) {
            // Nouvelle base temporelle GPS reçue, on la prend en compte pour faire avancer l'heure avec millis()
            last_time_sync_millis = millis();

            last_used_gps_time_local = utc_to_local(utc_day, utc_month, utc_year, last_got_gps_time);

            last_used_gps_time = last_got_gps_time;
        }

        if (last_used_gps_time_local.hours != 100 && last_used_gps_time_local.minutes != 100) {
            // Faire avancer l'heure en fonction du temps écoulé depuis la dernière synchronisation avec une base temporelle GPS
            unsigned long elapsed_minutes = (millis() - last_time_sync_millis) / 60000UL;
            uint16_t base_total_minutes = (uint16_t)last_used_gps_time_local.hours * 60 + last_used_gps_time_local.minutes;
            uint16_t current_total_minutes = (base_total_minutes + (uint16_t)elapsed_minutes) % (24 * 60);

            struct time_base calculated = {(uint8_t)(current_total_minutes / 60), (uint8_t)(current_total_minutes % 60)};
            if (calculated.hours != current_time.hours || calculated.minutes != current_time.minutes) {
                current_time.hours = calculated.hours;
                current_time.minutes = calculated.minutes;
                return 1;
            }
        }
    }
    return 0;
}
