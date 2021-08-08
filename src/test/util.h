//
//  util.h
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 31/07/2021.
//  Distributed under the terms of the LGPL-3.0
//
#ifndef SUNRISE_SUNSET_CALCULATOR_UTIL_H
#define SUNRISE_SUNSET_CALCULATOR_UTIL_H

#include <time.h>

#ifdef _WIN32
#define timegm _mkgmtime
#pragma warning(disable : 4996) // deprecations
#endif

static time_t time_t_for_time(int year, int month, int day, int hours, int mins) {
    struct tm tm;
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hours;
    tm.tm_min = mins;
    tm.tm_sec = 0;
    return timegm(&tm);
}

#define BRISTOL_LAT 51.4545
#define BRISTOL_LON -2.5879

#define STLOUIS_LAT 38.6272
#define STLOUIS_LON -90.1978

#define SVALBARD_LAT 79.0
#define SVALBARD_LON 17.0

#endif //SUNRISE_SUNSET_CALCULATOR_UTIL_H
