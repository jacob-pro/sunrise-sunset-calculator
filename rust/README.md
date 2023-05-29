# Sunrise Sunset Calculator for Rust

[![Build status](https://github.com/jacob-pro/sunrise-sunset-calculator/actions/workflows/rust.yml/badge.svg)](https://github.com/jacob-pro/sunrise-sunset-calculator/actions/workflows/rust.yml)
[![crates.io](https://img.shields.io/crates/v/sunrise-sunset-calculator.svg)](https://crates.io/crates/sunrise-sunset-calculator)
[![docs.rs](https://docs.rs/sunrise-sunset-calculator/badge.svg)](https://docs.rs/sunrise-sunset-calculator/latest/sunrise_sunset_calculator/)

A crate for computing sunrise and sunset times.

## Usage

```
let result = SunriseSunsetParameters::new(timestamp, latitude, longitude).calculate()?;
```

## Example

```
$ cargo run --example sunrise-sunset-calculator -- lookup --location london

Using timestamp: 1628381118
Using coordinates: 51.5073219, -0.1276474
Visible: false
Sun set at:     2021-08-07 20:37 (+01:00)
Sun rises at:   2021-08-08 05:35 (+01:00)
```

## Implementation Details

Internally this uses a port of [NREL's Solar Position Algorithm (SPA)](https://midcdmz.nrel.gov/spa/)
to compute the solar elevation / altitude at a given time. Dates between -2000 and 6000 are accepted.

We use the NOAA definition of sunrise/sunset as being at the point which the center of the sun is 0.8333° below
the horizon. We then use interval bisection to find the point at which the sun's elevation crosses this boundary.

It will work at all latitudes on Earth, although the step size option controls the shortest day/night lengths that
will be detected, which is configured with a reasonable default based on the input latitude.

## License

All my code is LGPL, but the NREL algorithm this bundles has its own separate license, so take this into account.
