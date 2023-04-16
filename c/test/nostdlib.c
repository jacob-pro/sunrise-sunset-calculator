//
//  nostdlib.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 01/08/2021.
//  Distributed under the terms of the LGPL-3.0
//
#include <ssc.h>
void _start() {
    while (1) {
        ssc_input input;
        ssc_result result;
        ssc(&input, &result);
    }
}
