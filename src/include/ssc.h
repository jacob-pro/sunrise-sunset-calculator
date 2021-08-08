//
//  ssc.h
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 31/07/2021.
//  Distributed under the terms of the LGPL-3.0
//
#ifndef SUNRISE_SUNSET_CALCULATOR_SSC_H
#define SUNRISE_SUNSET_CALCULATOR_SSC_H

#include "spa.h"
#include <stdbool.h>
#include <stdint.h>

typedef int64_t unix_t;

typedef struct {
    // Always required inputs
    unix_t time;      // Unix time
    double latitude;  // Observer latitude (negative south of equator)
    double longitude; // Observer longitude (negative west of Greenwich)

    // Defaults will be set when using `ssc_input_defaults`
    double delta_ut1;     // Fractional second difference between UTC and UT which is used
    double delta_t;       // Difference between earth rotation time and terrestrial time
    double elevation;     // Observer elevation [meters]
    double pressure;      // Annual average local pressure [millibars]
    double temperature;   // Annual average local temperature [degrees Celsius]
    double atmos_refract; // Atmospheric refraction at sunrise and sunset (0.5667 deg is typical)
    uint32_t step_size;   // Search step size
                          // It needs to be less than the length of the shortest day/night
                          // If it is too large there is a risk of missing event
                          // However too small will be slow, since more iterations will be done than necessary
                          // A default is set proportional to your latitude
} ssc_input;

void ssc_input_defaults(ssc_input *input, unix_t time, double latitude, double longitude);

typedef struct {
    unix_t set;
    unix_t rise;
    bool visible;
} ssc_result;

SpaStatus ssc(const ssc_input *input, ssc_result *result);

#endif //SUNRISE_SUNSET_CALCULATOR_SSC_H
