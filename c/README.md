# Sunrise Sunset Calculator

[![Build status](https://github.com/jacob-pro/sunrise-sunset-calculator/actions/workflows/cmake.yml/badge.svg)](https://github.com/jacob-pro/sunrise-sunset-calculator/actions/workflows/cmake.yml)

A C99 library for computing sunrise and sunset times. Builds are tested on Linux/GCC, Windows/MSVC and MacOS/Clang,
and there is also support for nostdlib environments.

## Usage

This library uses Unix time for input and output, on nearly all platforms (including Linux/Unix/Windows) this is
equivalent to `time_t` so you can just use the C time functions to work with dates. (If not consider looking at the
[musl](http://git.musl-libc.org/cgit/musl/tree/src/time) implementation of these functions)

```
#include "ssc.h"
...
SunriseSunsetParameters params;
SunriseSunsetResult result;
SunriseSunsetParameters_init(&params, UNIX_TIMESTAMP, LATITUDE, LONGITUDE);
assert(sunrise_sunset_calculate(&input, &result) == SpaStatus_Success);
printf("Visible: %s, Sunset: %lld, Sunrise: %lld", result.visible ? "true" : "false", result.set, result.rise);
```

The input timestamp is guaranteed to be between the output sunset and sunrise.

## Implementation Details

Internally this uses a stripped down version of [NREL's Solar Position Algorithm (SPA)](https://midcdmz.nrel.gov/spa/)
to compute the solar elevation / altitude at a given time. Dates between -2000 and 6000 are accepted.

We use the NOAA definition of sunrise/sunset as being at the point which the center of the sun is 0.8333° below
the horizon. We then use interval bisection to find the point at which the sun's elevation crosses this boundary.

It will work at all latitudes on Earth, although the step size option controls the shortest day/night lengths that
will be detected, which is configured with a reasonable default based on the input latitude.

## License

All my code is LGPL, but the NREL algorithm this bundles has its own separate license, so take this into account.
