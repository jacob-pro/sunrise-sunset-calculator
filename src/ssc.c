//
//  ssc.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 31/07/2021.
//

#include "ssc.h"

void ssc_input_defaults(ssc_input *input, uint64_t time, double latitude, double longitude) {
    input->time = time;
    input->latitude = latitude;
    input->longitude = longitude;
    input->delta_ut1 = 0.0;
    input->delta_t = 0.0;
    input->elevation = 0.0;
    input->pressure = 1013.25;
    input->temperature = 16.0;
    input->atmos_refract = 0.5667;
}

