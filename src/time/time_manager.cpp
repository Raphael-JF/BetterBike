#include "Arduino.h"
#include "time/time_manager.h"

// Base temporelle actuellement affichée à l'écran (en UTC)
struct time_base current_time = {100, 100};

// Dernière base temporelle GPS reçue
struct time_base last_got_gps_time = {100, 100};

// Dernière base temporelle GPS prise en compte pour faire avancer l'heure avec millis(). Elle est en UTC.
struct time_base last_used_gps_time = {100, 100};

// Dernière base temporelle GPS prise en compte pour faire avancer l'heure avec millis(). Elle est en fuseau local français.
struct time_base last_used_gps_time_local = {100, 100};

// Valeur de millis() au moment où gps_base_hour et gps_base_minutes ont été synchronisées
unsigned long gps_sync_millis = 0;

uint16_t utc_year = 2026;
uint8_t utc_month = 4;
uint8_t utc_day = 0;



void update_time() {
    if (last_got_gps_time.hours != 100 && last_got_gps_time.minutes != 100) {

        if (last_got_gps_time.minutes != last_used_gps_time.minutes || last_got_gps_time.hours != last_used_gps_time.hours) {
            gps_sync_millis = millis();

            last_used_gps_time_local = utc_to_local(utc_day, utc_month, utc_year, last_got_gps_time);

            last_used_gps_time = last_got_gps_time;
        }

        if (last_used_gps_time_local.hours != 100 && last_used_gps_time_local.minutes != 100) {
            unsigned long elapsed_minutes = (millis() - gps_sync_millis) / 60000UL;
            uint16_t base_total_minutes = (uint16_t)last_used_gps_time_local.hours * 60 + last_used_gps_time_local.minutes;
            uint16_t current_total_minutes = (base_total_minutes + (uint16_t)elapsed_minutes) % (24 * 60);

            current_time.hours = current_total_minutes / 60;
            current_time.minutes = current_total_minutes % 60;
        }
    }


    // unsigned long total_minutes = millis() / 60000;
    // uint8_t current_hours = (total_minutes / 60) % 24;
    // uint8_t current_minutes = total_minutes % 60;
    // lcd_respring_time(current_hours, current_minutes);
}




// Check if a year is a leap year
uint8_t is_leap_year(uint16_t year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

// Day of week (0 = Sunday, ..., 6 = Saturday)
// Zeller's congruence (simplified)
uint8_t day_of_week(uint8_t d, uint8_t m, uint16_t y) {
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    uint8_t K = y % 100;
    uint8_t J = y / 100;
    uint8_t h = (d + (13 * (m + 1)) / 5 + K + K/4 + J/4 + 5*J) % 7;
    return (h + 6) % 7; // convert to 0 = Sunday
}

// Get the date (day) of the last Sunday of a given month
uint8_t last_sunday(uint8_t month, uint16_t year) {
    uint8_t days_in_month;

    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            days_in_month = 31; break;
        case 4: case 6: case 9: case 11:
            days_in_month = 30; break;
        case 2:
            days_in_month = is_leap_year(year) ? 29 : 28;
            break;
    }

    uint8_t d = days_in_month;
    while (day_of_week(d, month, year) != 0) {
        d--;
    }
    return d;
}

// Determine if daylight saving time (DST) is active in France/Europe
uint8_t is_dst(uint8_t day, uint8_t month, uint16_t year, uint8_t hour_utc) {
    uint8_t march = last_sunday(3, year);
    uint8_t october = last_sunday(10, year);

    if (month < 3 || month > 10) return 0;
    if (month > 3 && month < 10) return 1;

    if (month == 3) {
        if (day > march) return 1;
        if (day < march) return 0;
        return (hour_utc >= 1); // switch at 1:00 UTC
    }

    if (month == 10) {
        if (day < october) return 1;
        if (day > october) return 0;
        return (hour_utc < 1); // switch at 1:00 UTC
    }

    return 0;
}

// Convert UTC to local time (France)
struct time_base utc_to_local(uint8_t day, uint8_t month, uint16_t year, struct time_base time_utc){
    uint8_t offset = is_dst(day, month, year, time_utc.hours) ? 2 : 1;
    struct time_base time_local;
    time_local.hours = time_utc.hours + offset;
    time_local.minutes = time_utc.minutes;

    if (time_local.hours >= 24) {
        time_local.hours -= 24;
        // optional: handle day/month/year rollover if needed
    }
    return time_local;
}