//
//  brightness.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 14/11/2018.
//
#include "brightness.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <stdbool.h>

/**
 * @param x - the current time
 * @param transition - the length of the transition period in seconds
 * @param event - the time of the sunset/rise in question
 * @param decreasing - set true if the function should be decreasing
 * @param low
 * @param high
 * @param brightnessPtr - brightness result
 * @param expiryPtr - expiry result
 */
static void sine_curve(time_t x,
                       time_t transition,
                       time_t event,
                       bool decreasing,
                       int low,
                       int high,
                       int *brightnessPtr,
                       int *expiryPtr
) {
    // We need to transform the sine function
    double Ymultiplier = (high - low) / 2.0;     // scale height to the difference between min and max brightness
    double Yoffset = Ymultiplier + low;          // shift upwards to the midpoint brightness
    double Xmultiplier = M_PI / transition;      // scale half a cycle to be equal to the transition time
    if (decreasing) Xmultiplier = -Xmultiplier;  // flip the curve for to make it a decreasing function
    time_t Xoffset = x - event;                  // shift rightwards to centre on the sunrise/sunset event
    double brightnessf = (Ymultiplier * sin(Xmultiplier * Xoffset)) + Yoffset;
    int brightness = (int) round(brightnessf);   // round to nearest integer brightness

    // Work out the expiry time; when the brightness will change to the next integer value
    int nextUpdateBrightness = decreasing ? brightness - 1 : brightness + 1;
    if (nextUpdateBrightness > high) nextUpdateBrightness = high;
    else if (nextUpdateBrightness < low) nextUpdateBrightness = low;
    if (x == event) {
        *expiryPtr = 1;        // Don't get stuck into an infinite loop when exactly on the boundary
    } else {
        // inverse the sine function at nextUpdateBrightness
        int offsetExpiry = (int) round(asin((nextUpdateBrightness - Yoffset) / Ymultiplier) / Xmultiplier);
        time_t expiryTime = offsetExpiry + event;
        *expiryPtr = expiryTime - x;
    }
    *brightnessPtr = brightness;
}

SSCBrightnessResult ssc_calculate_brightness(const SSCBrightnessParams *params, const SSCAroundTimeResult *result) {
    SSCBrightnessResult ret;
    int low = params->brightness_night;
    int high = params->brightness_day;
    int transitionSeconds = params->transition_mins * 60;    //time for transition from low to high
    int halfTransitionSeconds = transitionSeconds / 2;

    time_t A, B;
    if (result->visible) {      // Daytime
        A = result->rise + halfTransitionSeconds;  // When the sun rose this morning  + transition
        B = result->set - halfTransitionSeconds;   // Whe the sun sets this evening - transition
    } else {                    // Nighttime
        A = result->set + halfTransitionSeconds;   // When the sun set at the start of night + transition
		B = result->rise - halfTransitionSeconds;  // When the sun will rise again - transition
    }

    if (result->time < A) {
        time_t event = result->visible ? result->rise : result->set;
        sine_curve(result->time, transitionSeconds, event, !result->visible, low, high, &ret.brightness, &ret.expiry_seconds);

    } else if (result->time >= B) {        //greater or equal to or it would get stuck in a loop
        time_t event = result->visible ? result->set : result->rise;
        sine_curve(result->time, transitionSeconds, event, result->visible, low, high, &ret.brightness, &ret.expiry_seconds);

    } else {        // Time is >=A and <B, therefore the brightness next change is at B
        ret.brightness = (result->visible) ? high : low;
        ret.expiry_seconds = B - result->time;
    }

    return ret;
}


#ifdef TEST_BRIGHTNESS

#include "tinytest/tinytest.h"
#include "time_util.h"

void test_sunset_sine_curve() {

    int brightness;
    time_t expiry;
    int low = 40;
    int high = 80;
    time_t transitionTime = 60 * 60;	//60 minutes
    time_t fictional_sunset = time_t_for_time(2018, 12, 2, 16, 0, 0);
    int midpoint = (int) round(low + (high - low) / 2.0);

    //At start of the transition it should equal the day brightness
    time_t start_of_transition = time_t_for_time(2018, 12, 2, 15, 30, 0);
    sine_curve(start_of_transition, transitionTime, fictional_sunset, true, low, high, &brightness, &expiry);
    ASSERT_EQUALS((float)high, brightness);		//80

    //Test part way between transition. It should be less than the daytime brightness. But greater than the midpoint because it is not yet sunset
    time_t before_sunset = time_t_for_time(2018, 12, 2, 15, 45, 0);
    sine_curve(before_sunset, transitionTime, fictional_sunset, true, low, high, &brightness, &expiry);
    ASSERT("Must be in range", (brightness < high && brightness > midpoint));		//~74

    //At sunset it should be half way between the day and night brightness
    sine_curve(fictional_sunset, transitionTime, fictional_sunset, true, low, high, &brightness, &expiry);
    ASSERT_EQUALS(midpoint, brightness);		//60

    //At end of the transition it should equal the night brightness
    time_t end_of_transition = time_t_for_time(2018, 12, 2, 16, 30, 0);
    sine_curve(end_of_transition, transitionTime, fictional_sunset, true, low, high, &brightness, &expiry);
    ASSERT_EQUALS((float)low, brightness);		//40
}

void test_sunrise_sine_curve() {

    int brightness;
    time_t expiry;
    int low = 35;
    int high = 76;
    time_t transitionTime = 40 * 60;	//40 minutes
    time_t fictional_sunrise = time_t_for_time(2018, 12, 2, 8, 0, 0);
    int midpoint = (int) round(low + (high - low) / 2.0);

    //At start of the transition it should equal the night brightness
    time_t start_of_transition = time_t_for_time(2018, 12, 2, 7, 40, 0);
    sine_curve(start_of_transition, transitionTime, fictional_sunrise, false, low, high, &brightness, &expiry);
    ASSERT_EQUALS((float)low, brightness);			//35

    //Test part way between transition. It should be greater than night brighness. But less than the midpoint because it is not yet sunrise
    time_t before_sunrise = time_t_for_time(2018, 12, 2, 7, 50, 0);
    sine_curve(before_sunrise, transitionTime, fictional_sunrise, false, low, high, &brightness, &expiry);
    ASSERT("Must be in range", (brightness > low && brightness < midpoint));		//~41

    //At sunrise it should be half way between the day and night brightness
    sine_curve(fictional_sunrise, transitionTime, fictional_sunrise, false, low, high, &brightness, &expiry);
    ASSERT_EQUALS(midpoint, brightness);		//55.5

    //At end of the transition it should equal the daytime brightness
    time_t end_of_transition = time_t_for_time(2018, 12, 2, 8, 20, 0);
    sine_curve(end_of_transition, transitionTime, fictional_sunrise, false, low, high, &brightness, &expiry);
    ASSERT_EQUALS((float)high, brightness);		//76
}

// Check that time_t is in seconds on this platform
void test_platform() {
    time_t now;
    time(&now);
    struct tm now_tm;
    gmtime_r(&now, &now_tm);
    now_tm.tm_sec += 5;
    now += 5;
    ASSERT_EQUALS(mktime(&now_tm), now);
}

void test_whole_algorithm() {
    SSCBrightnessParams cfg;
    cfg.brightness_day = 90;
    cfg.brightness_night = 50;
    cfg.transition_mins = 120;

    SSCAroundTimeResult input;
    input.rise = time_t_for_time(2018, 11, 1, 8, 0, 0);
    input.set = time_t_for_time(2018, 11, 1, 17, 0, 0);
    input.visible = true;

    SSCBrightnessResult output;

    // TODO: These tests are not finished

    //Sunrise
    input.time = time_t_for_time(2018, 11, 1, 8, 0, 0);
    output = ssc_calculate_brightness(&cfg, &input);

    //Mid transition from sunrise
    input.time = time_t_for_time(2018, 11, 1, 8, 30, 0);
    output = ssc_calculate_brightness(&cfg, &input);

    //End transition from sunrise
    input.time = time_t_for_time(2018, 11, 1, 9, 0, 0);
    output = ssc_calculate_brightness(&cfg, &input);

    //Begin transition to sunset
    input.time = time_t_for_time(2018, 11, 1, 16, 0, 0);
    output = ssc_calculate_brightness(&cfg, &input);

    //Mid transition to sunset
    input.time = time_t_for_time(2018, 11, 1, 16, 30, 0);
    output = ssc_calculate_brightness(&cfg, &input);

    //Sunset
    input.time = time_t_for_time(2018, 11, 1, 17, 0, 0);
    output = ssc_calculate_brightness(&cfg, &input);

    printf("");
}

int main() {
    RUN(test_platform);
    RUN(test_sunset_sine_curve);
    RUN(test_sunrise_sine_curve);
    RUN(test_whole_algorithm);
    return TEST_REPORT();
}

#endif

#ifdef BRIGHTNESS_SIM

#include <stdio.h>
#include "time_util.h"
#include "gmtime.h"

void simulate_cycle() {
    SSCBrightnessParams cfg;
    cfg.brightness_day = 90;
    cfg.brightness_night = 50;
    cfg.transition_mins = 30;

    time_t now;
    time(&now);
    struct tm now_tm;
    gmtime_r(&now, &now_tm);

    time_t start = time_t_for_time(now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday, 12, 0, 0);
    time_t end = time_t_for_time(now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday + 1, 12, 0, 0);
    while (start <= end) {

        SSCAroundTimeResult ssr;        // Calculate the sunrise and sunset
        ssc_around_time(51.4545, -2.5879, start, &ssr);
        SSCBrightnessResult result = ssc_calculate_brightness(&cfg, &ssr);  //Calculate the brightness

        char strNow[100];
        struct tm *local = localtime(&start);
        strftime(strNow, sizeof(strNow), "%d/%m/%y %H:%M", local);

        char strSet[100];
        struct tm *set = localtime(&ssr.set);
        strftime(strSet, sizeof(strSet), "%d/%m/%y %H:%M", set);

        char strRise[100];
        struct tm *rise = localtime(&ssr.rise);
        strftime(strRise, sizeof(strRise), "%d/%m/%y %H:%M", rise);

        printf("At: %s \t Brightness: %d%% \t Set: %s \t Rise: %s (Expiry: %ds)\n", strNow, result.brightness, strSet, strRise, result.expiry_seconds);
        start = start + result.expiry_seconds;
    }
}

int main() {
    simulate_cycle();
}

#endif
