#include <stdint.h>

// Check if a year is a leap year
int is_leap_year(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

// Day of week (0 = Sunday, ..., 6 = Saturday)
// Zeller's congruence (simplified)
int day_of_week(int d, int m, int y) {
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    int K = y % 100;
    int J = y / 100;
    int h = (d + (13 * (m + 1)) / 5 + K + K/4 + J/4 + 5*J) % 7;
    return (h + 6) % 7; // convert to 0 = Sunday
}

// Get the date (day) of the last Sunday of a given month
int last_sunday(int month, int year) {
    int days_in_month;

    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            days_in_month = 31; break;
        case 4: case 6: case 9: case 11:
            days_in_month = 30; break;
        case 2:
            days_in_month = is_leap_year(year) ? 29 : 28;
            break;
    }

    int d = days_in_month;
    while (day_of_week(d, month, year) != 0) {
        d--;
    }
    return d;
}

// Determine if daylight saving time (DST) is active in France/Europe
int is_dst(int day, int month, int year, int hour_utc) {
    int march = last_sunday(3, year);
    int october = last_sunday(10, year);

    if (month < 3 || month > 10) return 0;
    if (month > 3 && month < 10) return 1;

    if (month == 3) {
        if (day > march) return 1;
        if (day < march) return 0;
        return (hour_utc >= 2); // switch at 2:00 UTC
    }

    if (month == 10) {
        if (day < october) return 1;
        if (day > october) return 0;
        return (hour_utc < 3); // switch at 3:00 UTC
    }

    return 0;
}

// Convert UTC to local time (France)
void utc_to_local(
    int day, int month, int year,
    int hour_utc, int minute_utc,
    int *hour_local, int *minute_local
) {
    int offset = is_dst(day, month, year, hour_utc) ? 2 : 1;

    *hour_local = hour_utc + offset;
    *minute_local = minute_utc;

    if (*hour_local >= 24) {
        *hour_local -= 24;
        // optional: handle day/month/year rollover if needed
    }
}