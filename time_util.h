//
//  test_util.h
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 02/12/2018.
//
#include "gmtime.h"
#include <time.h>

static time_t time_t_for_time(int year, int month, int day, int hours, int mins, int sec) {
    struct tm tmt;
    tmt.tm_year = year - 1900;
    tmt.tm_mon = month - 1;
    tmt.tm_mday = day;
    tmt.tm_hour = hours;
    tmt.tm_min = mins;
    tmt.tm_sec = sec;
    return timegm(&tmt);
}
