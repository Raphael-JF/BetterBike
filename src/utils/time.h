#ifndef TIME_H
#define TIME_H

#include <stdint.h>

struct time_base{
    uint8_t hours;
    uint8_t minutes;
};

// Check if a year is a leap year
uint8_t is_leap_year(uint16_t year);

// Day of week (0 = Sunday, ..., 6 = Saturday)
// Zeller's congruence (simplified)
uint8_t day_of_week(uint8_t d, uint8_t m, uint16_t y);

// Get the date (day) of the last Sunday of a given month
uint8_t last_sunday(uint8_t month, uint16_t year);

// Determine if daylight saving time (DST) is active in France/Europe
uint8_t is_dst(uint8_t day, uint8_t month, uint16_t year, uint8_t hour_utc);

// Convert UTC to local time (France)
struct time_base utc_to_local(uint8_t day, uint8_t month, uint16_t year, struct time_base time_utc);


#endif // TIME_H