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
    int brightness_night;     // Night brightness 0-100
    int brightness_day;    // Day brightness 0-100
    int transition_mins;     // Number of mins to transition between the two
} SSCBrightnessParams;

typedef struct {
    int brightness;         // The brightness as a percentage
    time_t expiry;             // When this result expires it should be recalculated
} SSCBrightnessResult;

// Calculates the brightness for a given configuration using given sunrise info.
// Use `ssc_around_time` to create a result at the desired time.
SSCBrightnessResult ssc_calculate_brightness(SSCBrightnessParams *params, SSCAroundTimeResult *result);

#endif
