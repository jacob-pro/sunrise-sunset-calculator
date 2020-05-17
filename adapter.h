//
//  adapter.h
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 19/11/2018.
//
#ifndef ADAPTER_H
#define ADAPTER_H

#include <time.h>
#include <stdbool.h>
#include "calculator.h"

typedef struct {
	time_t time;	//Time of result
	bool visible;	//If sun is visible
	time_t rise;	//The next/previous rise
	time_t set;		//The next/previous set
} SunriseSunsetResult;

// For a given time calculate if the sun is visible, and the next/previous sunrise and sunset
SSCalculationStatus SunriseSunsetAroundTime(double lat, double lon, time_t atTime, SunriseSunsetResult *result);

#endif
