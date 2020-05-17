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
    float brightnessNight;
    float brightnessDay;
    int transitionMins;
    float refreshDifference;
} SSBBrightnessParams;

typedef struct {
	float brightness;		//The brightness as a percentage
	time_t expiry;			//When this result expires it should be recalculated
} BrightnessAlgorithmResult;

// Calculates the brightness for a given configuration using given sunrise info.
// Use SunriseSunsetAdapter to create a result at the desired time.
// Note this will only work if platform time_t is in seconds
BrightnessAlgorithmResult CalculateBrightness(SSBBrightnessParams *params, SunriseSunsetResult *result);

#endif
