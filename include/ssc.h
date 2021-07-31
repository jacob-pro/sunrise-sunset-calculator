//
//  ssc.h
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 31/07/2021.
//
#ifndef SUNRISE_SUNSET_CALCULATOR_SSC_H
#define SUNRISE_SUNSET_CALCULATOR_SSC_H

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
} ssc_input;

void ssc_input_defaults(ssc_input *input, unix_t time, double latitude, double longitude);

typedef struct {
    unix_t set;
    unix_t rise;
    bool visible;
} ssc_result;

int ssc(ssc_input *input, ssc_result *result);

#endif //SUNRISE_SUNSET_CALCULATOR_SSC_H
