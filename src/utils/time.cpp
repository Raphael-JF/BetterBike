#include <stdint.h>

#include "time.h"


uint8_t is_leap_year(uint16_t year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

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