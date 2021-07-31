//
//  ssc.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 31/07/2021.
//
#include "ssc.h"
#define _USE_MATH_DEFINES
#include <math.h>

// This is approximate - goal here is too have large as possible step size without missing a rise/set
static uint32_t default_step_size(double latitude) {
    double shortest_day_hours_approx = (24.0 / M_PI) * acos(latitude / 66.0);
    int64_t seconds = (int64_t) (shortest_day_hours_approx * 3600) - 1800;
    if (seconds < 600 || fabs(latitude) > 66.0) {
        seconds = 600;
    }
    return (uint32_t) seconds;
}

void ssc_input_defaults(ssc_input *input, unix_t time, double latitude, double longitude) {
    input->time = time;
    input->latitude = latitude;
    input->longitude = longitude;
    input->delta_ut1 = 0.0;
    input->delta_t = 0.0;
    input->elevation = 0.0;
    input->pressure = 1013.25;
    input->temperature = 16.0;
    input->atmos_refract = 0.5667;
    input->step_size = default_step_size(latitude);
}

// https://stackoverflow.com/a/466348
static inline double jd_from_unix(unix_t t) {
    return (t / 86400.0) + 2440587.5;
}

// See Skyfield's general purpose implementation this is inspired by:
// https://github.com/skyfielders/python-skyfield/blob/aa59e2d4711c3a95804170889f138402edbf4237/skyfield/almanac.py#L216
// https://github.com/skyfielders/python-skyfield/blob/aa59e2d4711c3a95804170889f138402edbf4237/skyfield/searchlib.py#L12
static inline bool sun_is_up(spa_data *result) {
    return result->e >= -0.8333;
}

#define ENSURE_SPA_RESULT                                                                                              \
    if (spa_result != SpaStatus_Success) {                                                                             \
        return spa_result;                                                                                             \
    }

static int search_for_event(spa_data *data, unix_t start, int64_t step_size, bool target_upness, unix_t *result) {
    int spa_result;
    while (step_size != 0) {
        data->jd = jd_from_unix(start);
        spa_result = spa_calculate(data);
        ENSURE_SPA_RESULT;
        if (sun_is_up(data) == target_upness) {
            step_size = -(step_size / 2);
            target_upness = !target_upness;
        } else {
            start += step_size;
        }
    }
    *result = start;
    return 0;
}

SpaStatus ssc(ssc_input *input, ssc_result *result) {
    spa_data data;
    int spa_result;

    data.jd = jd_from_unix(input->time);
    data.delta_ut1 = input->delta_ut1;
    data.delta_t = input->delta_t;
    data.longitude = input->longitude;
    data.latitude = input->latitude;
    data.elevation = input->elevation;
    data.pressure = input->pressure;
    data.temperature = input->temperature;
    data.atmos_refract = input->atmos_refract;
    spa_result = spa_calculate(&data);
    if (spa_result) return spa_result;
    result->visible = sun_is_up(&data);

    unix_t *backward_time = result->visible ? &result->rise : &result->set;
    unix_t *forward_time = result->visible ? &result->set : &result->rise;
    int64_t step_signed = (int64_t) input->step_size;

    spa_result = search_for_event(&data, input->time, -step_signed, !result->visible, backward_time);
    ENSURE_SPA_RESULT;
    spa_result = search_for_event(&data, input->time, step_signed, !result->visible, forward_time);
    ENSURE_SPA_RESULT;

    return SpaStatus_Success;
}
