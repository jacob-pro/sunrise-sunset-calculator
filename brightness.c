//
//  brightness.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 14/11/2018.
//
#include "brightness.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <stdbool.h>

/**
 * @param x - the current time
 * @param transition - the length of the transition period in seconds
 * @param event - the time of the sunset/rise in question
 * @param decreasing - set true if the function should be decreasing
 * @param low
 * @param high
 * @param refreshDifference
 * @param brightnessPtr - brightness result
 * @param expiryPtr - expiry result
 */
void SSBAlgorithmSineCurve(time_t x, time_t transition, time_t event, bool decreasing, float low, float high, float refreshDifference, float *brightnessPtr, time_t *expiryPtr) {

	// We need to transform the sine function
	double Ymultiplier = (high - low) / 2.0;	//scale height to the difference between min and max brightness
	double Yoffset = Ymultiplier + low;		//shift upwards to the midpoint brightness
	double Xmultiplier = M_PI / transition;		//scale half a cycle to be equal to the transition time
	if (decreasing) Xmultiplier = -Xmultiplier;	//flip the curve for to make it a decreasing function
	time_t Xoffset = x - event;			//shift rightwards to centre on the sunrise/sunset event
	double brightness = (Ymultiplier * sin(Xmultiplier * Xoffset)) + Yoffset;

	//Work out the expiry time
	time_t expiry;
	if (decreasing) refreshDifference = -refreshDifference;
	double nextUpdateBrightness = brightness + refreshDifference;
	if (nextUpdateBrightness > high) nextUpdateBrightness = high;
	else if (nextUpdateBrightness < low) nextUpdateBrightness = low;
	if (x == event) {
		expiry = x + 1;		//don't get stuck into an infinite loop when on the boundary
	} else {
		//inverse the sine function at nextUpdateBrightness
		double offsetExpiry = asin((nextUpdateBrightness - Yoffset) / Ymultiplier) / Xmultiplier;
		expiry = offsetExpiry + event;
	}

	*brightnessPtr = (float)brightness;
	*expiryPtr = expiry;
}

BrightnessAlgorithmResult CalculateBrightness(SSBBrightnessParams *params, SunriseSunsetResult *result) {
	BrightnessAlgorithmResult ret;

	float low = params->brightnessNight;
	float high = params->brightnessDay;

	time_t transitionSeconds = params->transitionMins * 60;	//time for transition from low to high
	time_t halfTransitionSeconds = transitionSeconds / 2;
	time_t A, B;

	if (result->visible) {
		A = result->rise + halfTransitionSeconds;
		B = result->set - halfTransitionSeconds;
	} else {
		A = result->set + halfTransitionSeconds;
		B = result->rise - halfTransitionSeconds;
	}

	if (result->time < A) {
		time_t event = result->visible ? result->rise : result->set;
		SSBAlgorithmSineCurve(result->time, transitionSeconds, event, !result->visible, low, high, params->refreshDifference, &ret.brightness, &ret.expiry);

	} else if (result->time >= B) {		//greater or equal to or it would get stuck in a loop
		time_t event = result->visible ? result->set : result->rise;
		SSBAlgorithmSineCurve(result->time, transitionSeconds, event, result->visible, low, high, params->refreshDifference, &ret.brightness, &ret.expiry);

	} else {
		ret.brightness = (result->visible) ? high : low;
		ret.expiry = B;
	}
	
	return ret;
}


#ifdef TEST_BRIGHTNESS

#include "tinytest/tinytest.h"
#include "time_util.h"

void test_sunset_sine_curve() {

    float brightness;
    time_t expiry;
    float low = 40;
    float high = 80;
    float refresh = 1.0f;
    time_t transitionTime = 60 * 60;	//60 minutes
    time_t fictional_sunset = time_t_for_time(2018, 12, 2, 16, 0, 0);
    float midpoint = low + (high - low) / 2.0;

    //At start of the transition it should equal the day brightness
    time_t start_of_transition = time_t_for_time(2018, 12, 2, 15, 30, 0);
    SSBAlgorithmSineCurve(start_of_transition, transitionTime, fictional_sunset, true, low, high, refresh, &brightness, &expiry);
    ASSERT_EQUALS((float)high, brightness);		//80

    //Test part way between transition. It should be less than the daytime brightness. But greater than the midpoint because it is not yet sunset
    time_t before_sunset = time_t_for_time(2018, 12, 2, 15, 45, 0);
    SSBAlgorithmSineCurve(before_sunset, transitionTime, fictional_sunset, true, low, high, refresh, &brightness, &expiry);
    ASSERT("Must be in range", (brightness < high && brightness > midpoint));		//~74

    //At sunset it should be half way between the day and night brightness
    SSBAlgorithmSineCurve(fictional_sunset, transitionTime, fictional_sunset, true, low, high, refresh, &brightness, &expiry);
    ASSERT_EQUALS(midpoint, brightness);		//60

    //At end of the transition it should equal the night brightness
    time_t end_of_transition = time_t_for_time(2018, 12, 2, 16, 30, 0);
    SSBAlgorithmSineCurve(end_of_transition, transitionTime, fictional_sunset, true, low, high, refresh, &brightness, &expiry);
    ASSERT_EQUALS((float)low, brightness);		//40
}

void test_sunrise_sine_curve() {

    float brightness;
    time_t expiry;
    float low = 35;
    float high = 76;
    float refresh = 1.0f;
    time_t transitionTime = 40 * 60;	//40 minutes
    time_t fictional_sunrise = time_t_for_time(2018, 12, 2, 8, 0, 0);
    float midpoint = low + (high - low) / 2.0;

    //At start of the transition it should equal the night brightness
    time_t start_of_transition = time_t_for_time(2018, 12, 2, 7, 40, 0);
    SSBAlgorithmSineCurve(start_of_transition, transitionTime, fictional_sunrise, false, low, high, refresh, &brightness, &expiry);
    ASSERT_EQUALS((float)low, brightness);			//35

    //Test part way between transition. It should be greater than night brighness. But less than the midpoint because it is not yet sunrise
    time_t before_sunrise = time_t_for_time(2018, 12, 2, 7, 50, 0);
    SSBAlgorithmSineCurve(before_sunrise, transitionTime, fictional_sunrise, false, low, high, refresh, &brightness, &expiry);
    ASSERT("Must be in range", (brightness > low && brightness < midpoint));		//~41

    //At sunrise it should be half way between the day and night brightness
    SSBAlgorithmSineCurve(fictional_sunrise, transitionTime, fictional_sunrise, false, low, high, refresh, &brightness, &expiry);
    ASSERT_EQUALS(midpoint, brightness);		//55.5

    //At end of the transition it should equal the daytime brightness
    time_t end_of_transition = time_t_for_time(2018, 12, 2, 8, 20, 0);
    SSBAlgorithmSineCurve(end_of_transition, transitionTime, fictional_sunrise, false, low, high, refresh, &brightness, &expiry);
    ASSERT_EQUALS((float)high, brightness);		//76
}

void test_whole_algorithm() {
    SSBBrightnessParams cfg;
    cfg.brightnessDay = 90;
    cfg.brightnessNight = 50;
    cfg.transitionMins = 120;
    cfg.refreshDifference = 1.0f;

    SunriseSunsetResult input;
    input.rise = time_t_for_time(2018, 11, 1, 8, 0, 0);
    input.set = time_t_for_time(2018, 11, 1, 17, 0, 0);
    input.visible = true;

    BrightnessAlgorithmResult output;

    // TODO: These tests are not finished

    //Sunrise
    input.time = time_t_for_time(2018, 11, 1, 8, 0, 0);
    output = CalculateBrightness(&cfg, &input);

    //Mid transition from sunrise
    input.time = time_t_for_time(2018, 11, 1, 8, 30, 0);
    output = CalculateBrightness(&cfg, &input);

    //End transition from sunrise
    input.time = time_t_for_time(2018, 11, 1, 9, 0, 0);
    output = CalculateBrightness(&cfg, &input);

    //Begin transition to sunset
    input.time = time_t_for_time(2018, 11, 1, 16, 0, 0);
    output = CalculateBrightness(&cfg, &input);

    //Mid transition to sunset
    input.time = time_t_for_time(2018, 11, 1, 16, 30, 0);
    output = CalculateBrightness(&cfg, &input);

    //Sunset
    input.time = time_t_for_time(2018, 11, 1, 17, 0, 0);
    output = CalculateBrightness(&cfg, &input);

    printf("");
}

int main() {
    RUN(test_sunset_sine_curve);
    RUN(test_sunrise_sine_curve);
    RUN(test_whole_algorithm);
    return TEST_REPORT();
}

#endif

#ifdef BRIGHTNESS_SIM

#include <stdio.h>
#include "time_util.h"

void simulate_cycle() {
	SSBBrightnessParams cfg;
	cfg.brightnessDay = 90;
	cfg.brightnessNight = 50;
	cfg.transitionMins = 30;
	cfg.refreshDifference = 1.0;

	char strNow[100];
	char strSet[100];
	char strRise[100];

	time_t start = time_t_for_time(2018, 12, 02, 12, 0, 0);
	time_t end = time_t_for_time(2018, 12, 03, 12, 0, 0);
	while (start <= end) {

		SunriseSunsetResult ssr;		// Calculate the sunrise and sunset
		SunriseSunsetAroundTime(51.4545, -2.5879, start, &ssr);
		BrightnessAlgorithmResult result = CalculateBrightness(&cfg, &ssr);  //Calculate the brightness
		struct tm *local = localtime(&start);
		strftime(&strNow, 100, "%d/%m/%y %H:%M", local);
		struct tm *set = localtime(&ssr.set);
		strftime(&strSet, 100, "%d/%m/%y %H:%M", set);
		struct tm *rise = localtime(&ssr.rise);
		strftime(&strRise, 100, "%d/%m/%y %H:%M", rise);
		printf("At: %s \t Brightness: %f%% \t Set: %s \t Rise: %s \n", strNow, result.brightness, strSet, strRise);
		start = result.expiry;

	}
}

int main() {
	simulate_cycle();
}

#endif
