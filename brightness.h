//
//  brightness.h
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 14/11/2018.
//
#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include "adapter.h"
#include <time.h>

typedef struct {
	unsigned int brightness_night;    // Night brightness 0-100
    unsigned int brightness_day;      // Day brightness 0-100
	unsigned int transition_mins;     // Number of minutes to transition between the two
} SSCBrightnessParams;

typedef struct {
	unsigned int brightness;         // The brightness as a percentage
	unsigned int expiry_seconds;     // How many seconds to wait before recalculating
} SSCBrightnessResult;

// Calculates the brightness for a given configuration using given sunrise info.
// Use `ssc_around_time` to create a result at the desired time.
SSCBrightnessResult ssc_calculate_brightness(const SSCBrightnessParams *params, const SSCAroundTimeResult *result);

#endif
