//
//  ssc.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 31/07/2021.
//  Distributed under the terms of the LGPL-3.0
//
#include "ssc.h"
#define _USE_MATH_DEFINES
#include <math.h>

uint32_t sunrise_sunset_default_step_size(double latitude) {
    double latitude_abs = fabs(latitude);
    if (latitude_abs < 60.0) {
        return 14400;
    } else if (latitude_abs < 64.0) {
        return 3600;
    } else {
        return 600;
    }
}

void SunriseSunsetParameters_init(SunriseSunsetParameters *params, unix_t time, double latitude, double longitude) {
    params->time = time;
    params->latitude = latitude;
    params->longitude = longitude;
    params->delta_t = 0.0;
    params->elevation = SSC_DEFAULT_ELEVATION;
    params->pressure = SSC_DEFAULT_PRESSURE;
    params->temperature = SSC_DEFAULT_TEMPERATURE;
    params->atmos_refract = SSC_DEFAULT_ATMOSPHERIC_REFRACTION;
    params->step_size = sunrise_sunset_default_step_size(latitude);
}

/// Convert a Unix timestamp to Julian Day
/// @see <a href="https://stackoverflow.com/a/466348">Stack Overflow</a>
static inline double jd_from_unix(unix_t t) {
    return ((double) t / 86400.0) + 2440587.5;
}

/// Return true if the sun is currently visible
/// @see <a href="https://github.com/skyfielders/python-skyfield/blob/aa59e2d4711c3a95804170889f138402edbf4237/skyfield/almanac.py#L239">Skyfield implementation</a>
/// @param[in] result Results from solar position algorithm
static inline bool sun_is_up(const spa_data *result) {
    return result->e >= -0.8333;
}

#define ENSURE_SPA_RESULT(res)                                                                                         \
    if (res != SpaError_Success) {                                                                                     \
        return res;                                                                                                    \
    }

/// Find the next time when the solar visibility changes
/// @param[in, out] data Solar position algorithm parameters
/// @param start Unix timestamp to start search from
/// @param step_size Step size in seconds. A negative step size will search backwards
/// @param currently_visible True if the sun is currently visible at the start time
/// @param[out] result Out parameter to store timestamp of next event
/// @return SpaError code
static SpaError search_for_change_in_visibility(spa_data *data,
                                                unix_t start,
                                                int64_t step_size,
                                                bool currently_visible,
                                                unix_t *result) {
    int spa_result;
    while (step_size != 0) {
        data->jd = jd_from_unix(start);
        spa_result = spa_calculate(data);
        ENSURE_SPA_RESULT(spa_result);
        if (sun_is_up(data) != currently_visible) {
            step_size = -(step_size / 2);
            currently_visible = !currently_visible;
        } else {
            start += step_size;
        }
    }
    *result = start;
    return SpaError_Success;
}

SpaError sunrise_sunset_calculate(const SunriseSunsetParameters *params, SunriseSunsetResult *result) {
    spa_data data;
    int spa_result;

    data.jd = jd_from_unix(params->time);
    data.delta_t = params->delta_t;
    data.longitude = params->longitude;
    data.latitude = params->latitude;
    data.elevation = params->elevation;
    data.pressure = params->pressure;
    data.temperature = params->temperature;
    data.atmos_refract = params->atmos_refract;

    // Determine current visibility at start time
    spa_result = spa_calculate(&data);
    ENSURE_SPA_RESULT(spa_result);
    result->visible = sun_is_up(&data);

    unix_t *backward_out = result->visible ? &result->rise : &result->set;
    unix_t *forward_out = result->visible ? &result->set : &result->rise;
    int64_t step_signed = (int64_t) params->step_size;

    // Search backwards from start time
    spa_result = search_for_change_in_visibility(&data, params->time, -step_signed, result->visible, backward_out);
    ENSURE_SPA_RESULT(spa_result);
    // Search forwards from start time
    spa_result = search_for_change_in_visibility(&data, params->time, step_signed, result->visible, forward_out);
    ENSURE_SPA_RESULT(spa_result);

    return SpaError_Success;
}
