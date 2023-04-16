//
//  ssc_example.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 31/07/2021.
//  Distributed under the terms of the LGPL-3.0
//
#include "../test/util.h"
#include "ssc.h"
#include <assert.h>
#include <stdio.h>

// Run with Environment TZ=CST6CDT for local time
int main() {
    SunriseSunsetParameters input;
    SunriseSunsetResult result;

    time_t start = time_t_for_time(2021, 7, 28, 22, 0);
    time_t end = time_t_for_time(2021, 7, 29, 14, 0);

    while (start <= end) {
        SunriseSunsetParameters_init(&input, start, STLOUIS_LAT, STLOUIS_LON);
        assert(sunrise_sunset_calculate(&input, &result) == SpaError_Success);

        char strNow[100];
        struct tm *local = localtime(&start);
        strftime(strNow, sizeof(strNow), "%d/%m/%y %H:%M", local);

        char strSet[100];
        struct tm *set = localtime(&result.set);
        strftime(strSet, sizeof(strSet), "%d/%m/%y %H:%M", set);

        char strRise[100];
        struct tm *rise = localtime(&result.rise);
        strftime(strRise, sizeof(strRise), "%d/%m/%y %H:%M", rise);

        if (result.rise < result.set) {
            printf("At: %s \t Visible: %s \t Rise: %s \t Set: %s\n",
                   strNow,
                   result.visible ? "true " : "false",
                   strRise,
                   strSet);
        } else {
            printf("At: %s \t Visible: %s \t Set: %s \t Rise: %s\n",
                   strNow,
                   result.visible ? "true " : "false",
                   strSet,
                   strRise);
        }

        start += 1800;
    }
}
