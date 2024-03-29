//
//  test_ssc.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 31/07/2021.
//  Distributed under the terms of the LGPL-3.0
//
#include "ssc.h"
#include "util.h"
#include <assert.h>
#include <tinytest.h>

#define ACCURACY_SECONDS 60

static inline bool test_times(unix_t expected, unix_t actual) {
    return llabs(expected - actual) < ACCURACY_SECONDS;
}

#define ASSERT_VALID_RESULT(result, r, s, v)                                                                           \
    ASSERT("Sunrise correct", test_times(r, result.rise));                                                             \
    ASSERT("Sunset correct", test_times(s, result.set));                                                               \
    ASSERT_EQUALS(v, result.visible)

// Check that time_t is unix time on this platform, otherwise the other tests won't work
static void test_platform() {
    ASSERT_EQUALS(1627749060, time_t_for_time(2021, 7, 31, 16, 31));
    ASSERT_EQUALS(0, time_t_for_time(1970, 1, 1, 0, 0));
}

static void test_bristol() {
    SunriseSunsetParameters input;
    SunriseSunsetResult result;
    SpaError status;

    //Early morning of Monday 18th, Sun is not visible. It set at 16:17 the day before, and is due to rise at 7:35
    time_t set18 = time_t_for_time(2018, 11, 18, 16, 17);  // 16:17 Sunset
    time_t early19 = time_t_for_time(2018, 11, 19, 5, 30); // 5:30 Early morning
    time_t rise19 = time_t_for_time(2018, 11, 19, 7, 35);  // 7:35 Sunrise
    SunriseSunsetParameters_init(&input, early19, BRISTOL_LAT, BRISTOL_LON);
    status = sunrise_sunset_calculate(&input, &result);
    ASSERT_EQUALS(SpaError_Success, status);
    ASSERT_VALID_RESULT(result, rise19, set18, false);

    //In the middle of Monday 18th, Sun is visible. It should have risen at 7:35 and be due to set at 16:16
    time_t midday19 = time_t_for_time(2018, 11, 19, 12, 0); // 12:00 Midday
    time_t set19 = time_t_for_time(2018, 11, 19, 16, 16);   // 16:16 Sunset
    SunriseSunsetParameters_init(&input, midday19, BRISTOL_LAT, BRISTOL_LON);
    status = sunrise_sunset_calculate(&input, &result);
    ASSERT_EQUALS(SpaError_Success, status);
    ASSERT_VALID_RESULT(result, rise19, set19, true);

    //In the evening of Monday 18th, Sun is not visible. It should have set at 16:16 and be due to rise tomorrow at 7:36
    time_t late19 = time_t_for_time(2018, 11, 19, 20, 25); // 20:25 Evening
    time_t rise20 = time_t_for_time(2018, 11, 20, 7, 36);  // 7:36 Sunrise
    SunriseSunsetParameters_init(&input, late19, BRISTOL_LAT, BRISTOL_LON);
    status = sunrise_sunset_calculate(&input, &result);
    ASSERT_EQUALS(SpaError_Success, status);
    ASSERT_VALID_RESULT(result, rise20, set19, false);
}

static void test_outer_bounds_impl(time_t start, double lat, double lon, unsigned int days, unsigned int incr) {
    time_t end = start + (days * 24 * 60 * 60);
    SunriseSunsetParameters input;
    SunriseSunsetResult result;
    SpaError status;
    while (start <= end) {
        SunriseSunsetParameters_init(&input, start, lat, lon);
        status = sunrise_sunset_calculate(&input, &result);
        ASSERT_EQUALS(SpaError_Success, status);
        ASSERT("Bounded",
               ((result.rise <= start) && (start <= result.set)) || ((result.set <= start) && (start <= result.rise)));
        ASSERT_EQUALS((start > result.rise), result.visible);
        start += incr;
    }
}

// Check that the input time is always bounded by the given sunrise and sunset
static void test_outer_bounds() {
    time_t start = time_t_for_time(2021, 07, 28, 10, 0);
    test_outer_bounds_impl(start, STLOUIS_LAT, STLOUIS_LON, 10, 3600);
    test_outer_bounds_impl(start, BRISTOL_LAT, BRISTOL_LON, 10, 3600);

    time_t summer = time_t_for_time(2021, 07, 1, 0, 0);
    test_outer_bounds_impl(summer, SVALBARD_LAT, SVALBARD_LON, 5, 3600);
    time_t winter = time_t_for_time(2021, 01, 1, 0, 0);
    test_outer_bounds_impl(winter, SVALBARD_LAT, SVALBARD_LON, 5, 3600);
    time_t spring = time_t_for_time(2021, 02, 16, 0, 0);
    test_outer_bounds_impl(spring, SVALBARD_LAT, SVALBARD_LON, 10, 3600);
}

static void test_adelaide() {
    SunriseSunsetParameters input;
    SunriseSunsetResult result;
    SpaError status;

    uint32_t tz = (uint32_t) (10.5 * 60 * 60);                // UTC+10:30
    time_t sets = time_t_for_time(2021, 11, 13, 19, 57) - tz; // 19:57 sunset
    time_t mid = time_t_for_time(2021, 11, 13, 19, 00) - tz;  // evening
    time_t rose = time_t_for_time(2021, 11, 13, 6, 03) - tz;  // 6:03 rose
    SunriseSunsetParameters_init(&input, mid, ADELAIDE_LAT, ADELAIDE_LON);
    status = sunrise_sunset_calculate(&input, &result);
    ASSERT_EQUALS(SpaError_Success, status);
    ASSERT_VALID_RESULT(result, rose, sets, true);
}

int main() {
    RUN(test_platform);
    RUN(test_bristol);
    RUN(test_outer_bounds);
    RUN(test_adelaide);
    return TEST_REPORT();
}
