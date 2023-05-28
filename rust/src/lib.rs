//! Rust port of [Sunrise Sunset Calculator](https://github.com/jacob-pro/sunrise-sunset-calculator/tree/master/c)
//!
//! A library to calculate times of sunrise and sunset on Earth based on latitude/longitude.
#![deny(unsafe_code)]
pub mod spa;

use crate::spa::{SpaData, SpaError};

pub const SSC_DEFAULT_ATMOSPHERIC_REFRACTION: f64 = 0.5667;
pub const SSC_DEFAULT_TEMPERATURE: f64 = 16.0;
pub const SSC_DEFAULT_PRESSURE: f64 = 1013.25;
pub const SSC_DEFAULT_ELEVATION: f64 = 0.0;

/// Input parameters to the calculator
#[derive(Debug, Copy, Clone)]
pub struct SunriseSunsetParameters {
    /// Unix timestamp to calculate sunrise and sunset times around
    pub time: i64,
    /// The latitude (N) of the location to calculate for
    pub latitude: f64,
    /// The longitude (E) of the location to calculate for
    pub longitude: f64,
    /// Difference between earth rotation time and terrestrial time
    pub delta_t: f64,
    /// Observer elevation \[meters\]
    pub elevation: f64,
    /// Annual average local pressure \[millibars\]
    pub pressure: f64,
    /// Annual average local temperature \[degrees Celsius\]
    pub temperature: f64,
    /// Atmospheric refraction at sunrise and sunset
    pub atmos_refract: f64,
    /// Step size in seconds to use in the search.
    /// It should be less than the length of the shortest day or night or otherwise it is
    /// possible that a sunrise/sunset may be skipped.
    /// It should not be too small or otherwise or the search will take an unreasonable
    /// amount of time.
    pub step_size: u32,
}

impl SunriseSunsetParameters {
    /// Creates a [SunriseSunsetParameters] struct with required and default values.
    ///
    /// Uses [default_step_size()](Self::default_step_size) to generate step size.
    ///
    /// # Arguments
    ///
    /// * `time`: Unix timestamp to calculate sunrise and sunset times around
    /// * `latitude`: The latitude (N) of the location to calculate for
    /// * `longitude`: The longitude (E) of the location to calculate for
    pub fn new(time: i64, latitude: f64, longitude: f64) -> Self {
        SunriseSunsetParameters {
            time,
            latitude,
            longitude,
            delta_t: 0.0,
            elevation: SSC_DEFAULT_ELEVATION,
            pressure: SSC_DEFAULT_PRESSURE,
            temperature: SSC_DEFAULT_TEMPERATURE,
            atmos_refract: SSC_DEFAULT_ATMOSPHERIC_REFRACTION,
            step_size: Self::default_step_size(latitude),
        }
    }

    /// Calculate a sensible default step size for a given latitude
    ///
    ///  - Absolute latitude less than 60 = 4 hour step
    ///  - Absolute latitude less than 64 = 1 hour step
    ///  - Absolute latitude greater than 64 = 10 minute step
    ///
    /// Note that at extreme latitudes it is possible that very short days/nights may be skipped.
    /// See: [Shortest Day of the Year](http://time.unitarium.com/events/shortest-day.html)
    ///
    /// # Arguments
    ///
    /// * `latitude`: The latitude of the location to calculate sunrise and sunset times
    pub fn default_step_size(latitude: f64) -> u32 {
        let latitude_abs = latitude.abs();
        if latitude_abs < 60.0 {
            14400
        } else if latitude_abs < 64.0 {
            3600
        } else {
            600
        }
    }

    /// Calculate sunrise and sunset times using these parameters
    pub fn calculate(self) -> Result<SunriseSunsetResult, SpaError> {
        let mut data = SpaData {
            jd: jd_from_unix(self.time),
            delta_t: self.delta_t,
            longitude: self.longitude,
            latitude: self.latitude,
            elevation: self.elevation,
            pressure: self.pressure,
            temperature: self.temperature,
            atmos_refract: self.atmos_refract,
            ..Default::default()
        };
        data.calculate()?;

        let visible = sun_is_up(&data);
        let step_signed = self.step_size as i64;

        let backward_result =
            search_for_change_in_visibility(&mut data, self.time, -step_signed, visible)?;

        let forward_result =
            search_for_change_in_visibility(&mut data, self.time, step_signed, visible)?;

        Ok(SunriseSunsetResult {
            set: if visible {
                forward_result
            } else {
                backward_result
            },
            rise: if visible {
                backward_result
            } else {
                forward_result
            },
            visible,
        })
    }
}

/// Output values from the calculator
#[derive(Debug, Copy, Clone, Eq, PartialEq)]
pub struct SunriseSunsetResult {
    /// Unix timestamp of the closest sunset
    pub set: i64,
    /// Unix timestamp of the closest sunrise
    pub rise: i64,
    /// If the sun is currently visible
    pub visible: bool,
}

#[inline]
fn jd_from_unix(t: i64) -> f64 {
    t as f64 / 86400.0f64 + 2440587.5f64
}

#[inline]
fn sun_is_up(result: &SpaData) -> bool {
    result.e >= -0.8333f64
}

#[inline]
fn search_for_change_in_visibility(
    mut data: &mut SpaData,
    mut start: i64,
    mut step_size: i64,
    mut currently_visible: bool,
) -> Result<i64, SpaError> {
    while step_size != 0 {
        data.jd = jd_from_unix(start);
        data.calculate()?;
        if sun_is_up(data) != currently_visible {
            step_size = -(step_size / 2i64);
            currently_visible = !currently_visible;
        } else {
            start += step_size;
        }
    }
    Ok(start)
}

#[cfg(test)]
mod tests {
    use super::*;
    use approx::assert_abs_diff_eq;
    use chrono::{FixedOffset, TimeZone};

    const ACCURACY_SECONDS: i64 = 60;

    const BRISTOL_LAT: f64 = 51.4545;
    const BRISTOL_LON: f64 = -2.5879;

    const STLOUIS_LAT: f64 = 38.6272;
    const STLOUIS_LON: f64 = -90.1978;

    const SVALBARD_LAT: f64 = 79.0;
    const SVALBARD_LON: f64 = 17.0;

    const ADELAIDE_LAT: f64 = -34.92;
    const ADELAIDE_LON: f64 = 138.59;

    fn timestamp(year: i32, month: u32, day: u32, hour: u32, min: u32, tz_seconds: i32) -> i64 {
        let tz = FixedOffset::east_opt(tz_seconds).unwrap();
        tz.with_ymd_and_hms(year, month, day, hour, min, 0)
            .unwrap()
            .timestamp()
    }

    fn validate_result(
        res: &SunriseSunsetResult,
        expected_rise: i64,
        expected_set: i64,
        currently_visible: bool,
    ) {
        assert_abs_diff_eq!(res.rise, expected_rise, epsilon = ACCURACY_SECONDS);
        assert_abs_diff_eq!(res.set, expected_set, epsilon = ACCURACY_SECONDS);
        assert_eq!(
            res.visible, currently_visible,
            "Solar visibility did not match"
        );
    }

    fn calculate(timestamp: i64, latitude: f64, longitude: f64) -> SunriseSunsetResult {
        SunriseSunsetParameters::new(timestamp, latitude, longitude)
            .calculate()
            .unwrap()
    }

    #[test]
    fn test_bristol() {
        let early19 = timestamp(2018, 11, 19, 5, 30, 0);
        let res = calculate(early19, BRISTOL_LAT, BRISTOL_LON);
        let set18 = timestamp(2018, 11, 18, 16, 17, 0);
        let rise19 = timestamp(2018, 11, 19, 7, 35, 0);
        validate_result(&res, rise19, set18, false);

        let midday19 = timestamp(2018, 11, 19, 12, 0, 0);
        let res = calculate(midday19, BRISTOL_LAT, BRISTOL_LON);
        let set19 = timestamp(2018, 11, 19, 16, 16, 0);
        validate_result(&res, rise19, set19, true);

        let late19 = timestamp(2018, 11, 19, 20, 25, 0);
        let rise20 = timestamp(2018, 11, 20, 7, 36, 0);
        let res = calculate(late19, BRISTOL_LAT, BRISTOL_LON);
        validate_result(&res, rise20, set19, false);
    }

    fn test_outer_bounds_impl(
        mut start: i64,
        latitude: f64,
        longitude: f64,
        days: u32,
        increment: i64,
    ) {
        let end = start + (days as i64 * 24 * 60 * 60);
        let mut params = SunriseSunsetParameters::new(start, latitude, longitude);
        while start <= end {
            params.time = start;
            let res = params.calculate().unwrap();
            assert!(
                (res.rise <= start && start <= res.set) || (res.set <= start && start <= res.rise),
                "Sunrise and Sunset are not on either side of the input time"
            );
            assert_eq!((start > res.rise), res.visible, "Visibility incorrect");
            start += increment;
        }
    }

    #[test]
    fn test_outer_bounds() {
        let start = timestamp(2021, 07, 28, 10, 0, 0);
        test_outer_bounds_impl(start, STLOUIS_LAT, STLOUIS_LON, 10, 3600);
        test_outer_bounds_impl(start, BRISTOL_LAT, BRISTOL_LON, 10, 3600);

        let svalbard_summer = timestamp(2021, 07, 1, 0, 0, 0);
        test_outer_bounds_impl(svalbard_summer, SVALBARD_LAT, SVALBARD_LON, 5, 3600);

        let svalbard_winter = timestamp(2021, 01, 1, 0, 0, 0);
        test_outer_bounds_impl(svalbard_winter, SVALBARD_LAT, SVALBARD_LON, 5, 3600);

        let svalbard_spring = timestamp(2021, 02, 16, 0, 0, 0);
        test_outer_bounds_impl(svalbard_spring, SVALBARD_LAT, SVALBARD_LON, 10, 3600);
    }

    #[test]
    fn test_adelaide() {
        let tz = (10.5 * 60.0 * 60.0) as i32; // UTC+10:30
        let mid = timestamp(2021, 11, 13, 19, 00, tz);
        let res = calculate(mid, ADELAIDE_LAT, ADELAIDE_LON);
        let set = timestamp(2021, 11, 13, 19, 57, tz);
        let rise = timestamp(2021, 11, 13, 6, 03, tz);
        validate_result(&res, rise, set, true);
    }
}
