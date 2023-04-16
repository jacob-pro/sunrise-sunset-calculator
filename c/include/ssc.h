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

#define SSC_DEFAULT_ATMOSPHERIC_REFRACTION 0.5667
#define SSC_DEFAULT_TEMPERATURE 16
#define SSC_DEFAULT_PRESSURE 1013.25
#define SSC_DEFAULT_ELEVATION 0.0

typedef struct {
    unix_t time;          ///< Unix timestamp to calculate sunrise and sunset times around
    double latitude;      ///< The latitude (N) of the location to calculate for
    double longitude;     ///< The longitude (E) of the location to calculate for
    double delta_t;       ///< Difference between earth rotation time and terrestrial time
    double elevation;     ///< Observer elevation [meters]
    double pressure;      ///< Annual average local pressure [millibars]
    double temperature;   ///< Annual average local temperature [degrees Celsius]
    double atmos_refract; ///< Atmospheric refraction at sunrise and sunset
    uint32_t step_size;   ///< Step size in seconds to use in the search.
                          ///< It should be less than the length of the shortest day or night or otherwise it is
                          ///< possible that a sunrise/sunset may be skipped.
                          ///< It should not be too small or otherwise or the search will take an unreasonable
                          ///< amount of time.
} SunriseSunsetParameters;

/// Provides a sensible default step size for a given latitude
/// <ul>
///   <li> Absolute latitude less than 60 = 4 hour step
///   <li> Absolute latitude less than 64 = 1 hour step
///   <li> Absolute latitude greater than 64 = 10 minute step
/// </ul>
/// Note that at extreme latitudes it is possible that very short days/nights may be skipped.
/// @see <a href="http://time.unitarium.com/events/shortest-day.html">Shortest Day of The Year</a>
/// @param latitude The latitude of the location to calculate sunrise and sunset times
/// @return Step size in seconds
uint32_t sunrise_sunset_default_step_size(double latitude);

/// Initialise SunriseSunsetParameters with required and default values.
/// @param[out] params SunriseSunsetParameters struct to initialise
/// @param time Unix timestamp to calculate sunrise and sunset times around
/// @param latitude The latitude (N) of the location to calculate for
/// @param longitude The longitude (E) of the location to calculate for
void SunriseSunsetParameters_init(SunriseSunsetParameters *params, unix_t time, double latitude, double longitude);

typedef struct {
    unix_t set;   // Unix timestamp of the closest sunset
    unix_t rise;  // Unix timestamp of the closest sunrise
    bool visible; // If the sun is currently visible
} SunriseSunsetResult;

/// Calculate sunrise and sunset times
/// @param[in] params Input parameters
/// @param[out] result Struct to write results to
/// @return Result of the calculation
SpaError sunrise_sunset_calculate(const SunriseSunsetParameters *params, SunriseSunsetResult *result);

#endif //SUNRISE_SUNSET_CALCULATOR_SSC_H
