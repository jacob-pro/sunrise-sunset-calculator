#![cfg_attr(not(feature = "std"), no_std)]

#[allow(non_upper_case_globals)]
#[allow(non_camel_case_types)]
#[allow(non_snake_case)]
#[allow(dead_code)]
#[link(name = "ssc")]
pub mod binding;

use err_derive::Error;
use num_derive::FromPrimitive;
use num_traits::FromPrimitive;

#[derive(FromPrimitive, PartialEq, Debug)]
pub enum SscErrorCode {
    UnsupportedDate = binding::SpaStatus_SpaStatus_UnsupportedDate as isize,
    InvalidPressure = binding::SpaStatus_SpaStatus_InvalidPressure as isize,
    InvalidTemperature = binding::SpaStatus_SpaStatus_InvalidTemperature as isize,
    InvalidDeltaUt1 = binding::SpaStatus_SpaStatus_InvalidDeltaUt1 as isize,
    InvalidDeltaT = binding::SpaStatus_SpaStatus_InvalidDeltaT as isize,
    InvalidLongitude = binding::SpaStatus_SpaStatus_InvalidLongitude as isize,
    InvalidLatitude = binding::SpaStatus_SpaStatus_InvalidLatitude as isize,
    InvalidAtmosRefract = binding::SpaStatus_SpaStatus_InvalidAtmosRefract as isize,
    InvalidElevation = binding::SpaStatus_SpaStatus_InvalidElevation as isize,
}

#[derive(Debug, Error)]
pub enum SscError {
    #[error(display = "{:?}", _0)]
    KnownCode(SscErrorCode),
    #[error(display = "{}", _0)]
    UnknownCode(i32),
}

pub use binding::ssc_input as SscInput;
pub use binding::ssc_result as SscResult;

impl SscInput {
    pub fn new(time: i64, latitude: f64, longitude: f64) -> Self {
        let mut s = Self::default();
        unsafe {
            binding::ssc_input_defaults(&mut s, time, latitude, longitude);
        }
        s
    }

    pub fn compute(&self) -> Result<SscResult, SscError> {
        let mut result = SscResult::default();
        unsafe {
            let code = binding::ssc(self, &mut result);
            if code == binding::SpaStatus_SpaStatus_Success {
                Ok(result)
            } else {
                match FromPrimitive::from_i32(code as i32) {
                    Some(m) => Err(SscError::KnownCode(m)),
                    None => Err(SscError::UnknownCode(code as i32)),
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    pub use super::*;
    use chrono::{TimeZone, Utc};
    use num_traits::abs;

    const BRISTOL_LAT: f64 = 51.4545;
    const BRISTOL_LON: f64 = -2.5879;

    #[test]
    fn it_works() {
        let expected_set = Utc.ymd(2018, 11, 18).and_hms(16, 17, 0).timestamp();
        let early19 = Utc.ymd(2018, 11, 19).and_hms(5, 30, 0).timestamp();
        let expected_rise = Utc.ymd(2018, 11, 19).and_hms(7, 35, 0).timestamp();

        let input = SscInput::new(early19, BRISTOL_LAT, BRISTOL_LON);
        let result = input.compute().unwrap();
        assert_eq!(result.visible, false);
        assert!(abs(expected_set - result.set) < 60);
        assert!(abs(expected_rise - result.rise) < 60);
    }
}
