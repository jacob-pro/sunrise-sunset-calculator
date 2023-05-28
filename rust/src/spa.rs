//! Rust port of Solar Position Algorithm
//!
//! <https://midcdmz.nrel.gov/spa/spa.h>
//!
//! Converted with help from [c2rust](https://github.com/immunant/c2rust)

#![allow(unused_assignments, clippy::approx_constant)]
use core::f64::consts::PI;
use thiserror::Error;

/// An error that is returned for invalid input parameters
#[derive(Copy, Clone, Debug, Error)]
pub enum SpaError {
    #[error("Elevation out of range")]
    InvalidElevation,
    #[error("Atmospheric Refraction out of range")]
    InvalidAtmosRefract,
    #[error("Latitude out of range")]
    InvalidLatitude,
    #[error("Longitude out of range")]
    InvalidLongitude,
    #[error("Delta T out of range")]
    InvalidDeltaT,
    #[error("Temperature out of range")]
    InvalidTemperature,
    #[error("Pressure out of range")]
    InvalidPressure,
    #[error("Timestamp outside of supported range")]
    UnsupportedDate,
}

/// Input parameters to the Solar Position Algorithm
///
/// See [spa.h](https://github.com/jacob-pro/sunrise-sunset-calculator/blob/master/c/include/spa.h)
/// for full documentation.
#[derive(Copy, Clone, Default, Debug)]
pub struct SpaData {
    pub jd: f64,
    pub delta_t: f64,
    pub longitude: f64,
    pub latitude: f64,
    pub elevation: f64,
    pub pressure: f64,
    pub temperature: f64,
    pub atmos_refract: f64,
    pub jc: f64,
    pub jde: f64,
    pub jce: f64,
    pub jme: f64,
    pub l: f64,
    pub b: f64,
    pub r: f64,
    pub theta: f64,
    pub beta: f64,
    pub x0: f64,
    pub x1: f64,
    pub x2: f64,
    pub x3: f64,
    pub x4: f64,
    pub del_psi: f64,
    pub del_epsilon: f64,
    pub epsilon0: f64,
    pub epsilon: f64,
    pub del_tau: f64,
    pub lamda: f64,
    pub nu0: f64,
    pub nu: f64,
    pub alpha: f64,
    pub delta: f64,
    pub h: f64,
    pub xi: f64,
    pub del_alpha: f64,
    pub delta_prime: f64,
    pub h_prime: f64,
    pub e0: f64,
    pub del_e: f64,
    pub e: f64,
}

const TERM_A: u32 = 0;
const TERM_B: u32 = 1;
const TERM_C: u32 = 2;

const TERM_X0: u32 = 0;
const TERM_X1: u32 = 1;
const TERM_X2: u32 = 2;
const TERM_X3: u32 = 3;
const TERM_X4: u32 = 4;
const TERM_X_COUNT: u32 = 5;

const TERM_PSI_A: u32 = 0;
const TERM_PSI_B: u32 = 1;
const TERM_EPS_C: u32 = 2;
const TERM_EPS_D: u32 = 3;

const L_SUBCOUNT: [i32; 6] = [64, 34, 20, 7, 3, 1];

const B_SUBCOUNT: [i32; 2] = [5, 2];

const R_SUBCOUNT: [i32; 5] = [40, 10, 6, 2, 1];

const L_TERMS: [[[f64; 3]; 64]; 6] = [
    [
        [175347046.0f64, 0f64, 0f64],
        [3341656.0f64, 4.6692568f64, 6283.07585f64],
        [34894.0f64, 4.6261f64, 12566.1517f64],
        [3497.0f64, 2.7441f64, 5753.3849f64],
        [3418.0f64, 2.8289f64, 3.5231f64],
        [3136.0f64, 3.6277f64, 77713.7715f64],
        [2676.0f64, 4.4181f64, 7860.4194f64],
        [2343.0f64, 6.1352f64, 3930.2097f64],
        [1324.0f64, 0.7425f64, 11506.7698f64],
        [1273.0f64, 2.0371f64, 529.691f64],
        [1199.0f64, 1.1096f64, 1577.3435f64],
        [990f64, 5.233f64, 5884.927f64],
        [902f64, 2.045f64, 26.298f64],
        [857f64, 3.508f64, 398.149f64],
        [780f64, 1.179f64, 5223.694f64],
        [753f64, 2.533f64, 5507.553f64],
        [505f64, 4.583f64, 18849.228f64],
        [492f64, 4.205f64, 775.523f64],
        [357f64, 2.92f64, 0.067f64],
        [317f64, 5.849f64, 11790.629f64],
        [284f64, 1.899f64, 796.298f64],
        [271f64, 0.315f64, 10977.079f64],
        [243f64, 0.345f64, 5486.778f64],
        [206f64, 4.806f64, 2544.314f64],
        [205f64, 1.869f64, 5573.143f64],
        [202f64, 2.458f64, 6069.777f64],
        [156f64, 0.833f64, 213.299f64],
        [132f64, 3.411f64, 2942.463f64],
        [126f64, 1.083f64, 20.775f64],
        [115f64, 0.645f64, 0.98f64],
        [103f64, 0.636f64, 4694.003f64],
        [102f64, 0.976f64, 15720.839f64],
        [102f64, 4.267f64, 7.114f64],
        [99f64, 6.21f64, 2146.17f64],
        [98f64, 0.68f64, 155.42f64],
        [86f64, 5.98f64, 161000.69f64],
        [85f64, 1.3f64, 6275.96f64],
        [85f64, 3.67f64, 71430.7f64],
        [80f64, 1.81f64, 17260.15f64],
        [79f64, 3.04f64, 12036.46f64],
        [75f64, 1.76f64, 5088.63f64],
        [74f64, 3.5f64, 3154.69f64],
        [74f64, 4.68f64, 801.82f64],
        [70f64, 0.83f64, 9437.76f64],
        [62f64, 3.98f64, 8827.39f64],
        [61f64, 1.82f64, 7084.9f64],
        [57f64, 2.78f64, 6286.6f64],
        [56f64, 4.39f64, 14143.5f64],
        [56f64, 3.47f64, 6279.55f64],
        [52f64, 0.19f64, 12139.55f64],
        [52f64, 1.33f64, 1748.02f64],
        [51f64, 0.28f64, 5856.48f64],
        [49f64, 0.49f64, 1194.45f64],
        [41f64, 5.37f64, 8429.24f64],
        [41f64, 2.4f64, 19651.05f64],
        [39f64, 6.17f64, 10447.39f64],
        [37f64, 6.04f64, 10213.29f64],
        [37f64, 2.57f64, 1059.38f64],
        [36f64, 1.71f64, 2352.87f64],
        [36f64, 1.78f64, 6812.77f64],
        [33f64, 0.59f64, 17789.85f64],
        [30f64, 0.44f64, 83996.85f64],
        [30f64, 2.74f64, 1349.87f64],
        [25f64, 3.16f64, 4690.48f64],
    ],
    [
        [628331966747.0f64, 0f64, 0f64],
        [206059.0f64, 2.678235f64, 6283.07585f64],
        [4303.0f64, 2.6351f64, 12566.1517f64],
        [425.0f64, 1.59f64, 3.523f64],
        [119.0f64, 5.796f64, 26.298f64],
        [109.0f64, 2.966f64, 1577.344f64],
        [93f64, 2.59f64, 18849.23f64],
        [72f64, 1.14f64, 529.69f64],
        [68f64, 1.87f64, 398.15f64],
        [67f64, 4.41f64, 5507.55f64],
        [59f64, 2.89f64, 5223.69f64],
        [56f64, 2.17f64, 155.42f64],
        [45f64, 0.4f64, 796.3f64],
        [36f64, 0.47f64, 775.52f64],
        [29f64, 2.65f64, 7.11f64],
        [21f64, 5.34f64, 0.98f64],
        [19f64, 1.85f64, 5486.78f64],
        [19f64, 4.97f64, 213.3f64],
        [17f64, 2.99f64, 6275.96f64],
        [16f64, 0.03f64, 2544.31f64],
        [16f64, 1.43f64, 2146.17f64],
        [15f64, 1.21f64, 10977.08f64],
        [12f64, 2.83f64, 1748.02f64],
        [12f64, 3.26f64, 5088.63f64],
        [12f64, 5.27f64, 1194.45f64],
        [12f64, 2.08f64, 4694f64],
        [11f64, 0.77f64, 553.57f64],
        [10f64, 1.3f64, 6286.6f64],
        [10f64, 4.24f64, 1349.87f64],
        [9f64, 2.7f64, 242.73f64],
        [9f64, 5.64f64, 951.72f64],
        [8f64, 5.3f64, 2352.87f64],
        [6f64, 2.65f64, 9437.76f64],
        [6f64, 4.67f64, 4690.48f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
    [
        [52919.0f64, 0f64, 0f64],
        [8720.0f64, 1.0721f64, 6283.0758f64],
        [309.0f64, 0.867f64, 12566.152f64],
        [27f64, 0.05f64, 3.52f64],
        [16f64, 5.19f64, 26.3f64],
        [16f64, 3.68f64, 155.42f64],
        [10f64, 0.76f64, 18849.23f64],
        [9f64, 2.06f64, 77713.77f64],
        [7f64, 0.83f64, 775.52f64],
        [5f64, 4.66f64, 1577.34f64],
        [4f64, 1.03f64, 7.11f64],
        [4f64, 3.44f64, 5573.14f64],
        [3f64, 5.14f64, 796.3f64],
        [3f64, 6.05f64, 5507.55f64],
        [3f64, 1.19f64, 242.73f64],
        [3f64, 6.12f64, 529.69f64],
        [3f64, 0.31f64, 398.15f64],
        [3f64, 2.28f64, 553.57f64],
        [2f64, 4.38f64, 5223.69f64],
        [2f64, 3.75f64, 0.98f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
    [
        [289.0f64, 5.844f64, 6283.076f64],
        [35f64, 0f64, 0f64],
        [17f64, 5.49f64, 12566.15f64],
        [3f64, 5.2f64, 155.42f64],
        [1f64, 4.72f64, 3.52f64],
        [1f64, 5.3f64, 18849.23f64],
        [1f64, 5.97f64, 242.73f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
    [
        [114.0f64, 3.142f64, 0f64],
        [8f64, 4.13f64, 6283.08f64],
        [1f64, 3.84f64, 12566.15f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
    [
        [1f64, 3.14f64, 0f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
];

const B_TERMS: [[[f64; 3]; 5]; 2] = [
    [
        [280.0f64, 3.199f64, 84334.662f64],
        [102.0f64, 5.422f64, 5507.553f64],
        [80f64, 3.88f64, 5223.69f64],
        [44f64, 3.7f64, 2352.87f64],
        [32f64, 4f64, 1577.34f64],
    ],
    [
        [9f64, 3.9f64, 5507.55f64],
        [6f64, 1.73f64, 5223.69f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
];

const R_TERMS: [[[f64; 3]; 40]; 5] = [
    [
        [100013989.0f64, 0f64, 0f64],
        [1670700.0f64, 3.0984635f64, 6283.07585f64],
        [13956.0f64, 3.05525f64, 12566.1517f64],
        [3084.0f64, 5.1985f64, 77713.7715f64],
        [1628.0f64, 1.1739f64, 5753.3849f64],
        [1576.0f64, 2.8469f64, 7860.4194f64],
        [925.0f64, 5.453f64, 11506.77f64],
        [542.0f64, 4.564f64, 3930.21f64],
        [472.0f64, 3.661f64, 5884.927f64],
        [346.0f64, 0.964f64, 5507.553f64],
        [329.0f64, 5.9f64, 5223.694f64],
        [307.0f64, 0.299f64, 5573.143f64],
        [243.0f64, 4.273f64, 11790.629f64],
        [212.0f64, 5.847f64, 1577.344f64],
        [186.0f64, 5.022f64, 10977.079f64],
        [175.0f64, 3.012f64, 18849.228f64],
        [110.0f64, 5.055f64, 5486.778f64],
        [98f64, 0.89f64, 6069.78f64],
        [86f64, 5.69f64, 15720.84f64],
        [86f64, 1.27f64, 161000.69f64],
        [65f64, 0.27f64, 17260.15f64],
        [63f64, 0.92f64, 529.69f64],
        [57f64, 2.01f64, 83996.85f64],
        [56f64, 5.24f64, 71430.7f64],
        [49f64, 3.25f64, 2544.31f64],
        [47f64, 2.58f64, 775.52f64],
        [45f64, 5.54f64, 9437.76f64],
        [43f64, 6.01f64, 6275.96f64],
        [39f64, 5.36f64, 4694f64],
        [38f64, 2.39f64, 8827.39f64],
        [37f64, 0.83f64, 19651.05f64],
        [37f64, 4.9f64, 12139.55f64],
        [36f64, 1.67f64, 12036.46f64],
        [35f64, 1.84f64, 2942.46f64],
        [33f64, 0.24f64, 7084.9f64],
        [32f64, 0.18f64, 5088.63f64],
        [32f64, 1.78f64, 398.15f64],
        [28f64, 1.21f64, 6286.6f64],
        [28f64, 1.9f64, 6279.55f64],
        [26f64, 4.59f64, 10447.39f64],
    ],
    [
        [103019.0f64, 1.10749f64, 6283.07585f64],
        [1721.0f64, 1.0644f64, 12566.1517f64],
        [702.0f64, 3.142f64, 0f64],
        [32f64, 1.02f64, 18849.23f64],
        [31f64, 2.84f64, 5507.55f64],
        [25f64, 1.32f64, 5223.69f64],
        [18f64, 1.42f64, 1577.34f64],
        [10f64, 5.91f64, 10977.08f64],
        [9f64, 1.42f64, 6275.96f64],
        [9f64, 0.27f64, 5486.78f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
    [
        [4359.0f64, 5.7846f64, 6283.0758f64],
        [124.0f64, 5.579f64, 12566.152f64],
        [12f64, 3.14f64, 0f64],
        [9f64, 3.63f64, 77713.77f64],
        [6f64, 1.87f64, 5573.14f64],
        [3f64, 5.47f64, 18849.23f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
    [
        [145.0f64, 4.273f64, 6283.076f64],
        [7f64, 3.92f64, 12566.15f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
    [
        [4f64, 2.56f64, 6283.08f64],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
        [0.; 3],
    ],
];

const Y_TERMS: [[i32; 5]; 63] = [
    [0, 0, 0, 0, 1],
    [-2, 0, 0, 2, 2],
    [0, 0, 0, 2, 2],
    [0, 0, 0, 0, 2],
    [0, 1, 0, 0, 0],
    [0, 0, 1, 0, 0],
    [-2, 1, 0, 2, 2],
    [0, 0, 0, 2, 1],
    [0, 0, 1, 2, 2],
    [-2, -1, 0, 2, 2],
    [-2, 0, 1, 0, 0],
    [-2, 0, 0, 2, 1],
    [0, 0, -1, 2, 2],
    [2, 0, 0, 0, 0],
    [0, 0, 1, 0, 1],
    [2, 0, -1, 2, 2],
    [0, 0, -1, 0, 1],
    [0, 0, 1, 2, 1],
    [-2, 0, 2, 0, 0],
    [0, 0, -2, 2, 1],
    [2, 0, 0, 2, 2],
    [0, 0, 2, 2, 2],
    [0, 0, 2, 0, 0],
    [-2, 0, 1, 2, 2],
    [0, 0, 0, 2, 0],
    [-2, 0, 0, 2, 0],
    [0, 0, -1, 2, 1],
    [0, 2, 0, 0, 0],
    [2, 0, -1, 0, 1],
    [-2, 2, 0, 2, 2],
    [0, 1, 0, 0, 1],
    [-2, 0, 1, 0, 1],
    [0, -1, 0, 0, 1],
    [0, 0, 2, -2, 0],
    [2, 0, -1, 2, 1],
    [2, 0, 1, 2, 2],
    [0, 1, 0, 2, 2],
    [-2, 1, 1, 0, 0],
    [0, -1, 0, 2, 2],
    [2, 0, 0, 2, 1],
    [2, 0, 1, 0, 0],
    [-2, 0, 2, 2, 2],
    [-2, 0, 1, 2, 1],
    [2, 0, -2, 0, 1],
    [2, 0, 0, 0, 1],
    [0, -1, 1, 0, 0],
    [-2, -1, 0, 2, 1],
    [-2, 0, 0, 0, 1],
    [0, 0, 2, 2, 1],
    [-2, 0, 2, 0, 1],
    [-2, 1, 0, 2, 1],
    [0, 0, 1, -2, 0],
    [-1, 0, 1, 0, 0],
    [-2, 1, 0, 0, 0],
    [1, 0, 0, 0, 0],
    [0, 0, 1, 2, 0],
    [0, 0, -2, 2, 2],
    [-1, -1, 1, 0, 0],
    [0, 1, 1, 0, 0],
    [0, -1, 1, 2, 2],
    [2, -1, -1, 2, 2],
    [0, 0, 3, 2, 2],
    [2, -1, 0, 2, 2],
];

const PE_TERMS: [[f64; 4]; 63] = [
    [-171996f64, -174.2f64, 92025f64, 8.9f64],
    [-13187f64, -1.6f64, 5736f64, -3.1f64],
    [-2274f64, -0.2f64, 977f64, -0.5f64],
    [2062f64, 0.2f64, -895f64, 0.5f64],
    [1426f64, -3.4f64, 54f64, -0.1f64],
    [712f64, 0.1f64, -7f64, 0f64],
    [-517f64, 1.2f64, 224f64, -0.6f64],
    [-386f64, -0.4f64, 200f64, 0f64],
    [-301f64, 0f64, 129f64, -0.1f64],
    [217f64, -0.5f64, -95f64, 0.3f64],
    [-158f64, 0f64, 0f64, 0f64],
    [129f64, 0.1f64, -70f64, 0f64],
    [123f64, 0f64, -53f64, 0f64],
    [63f64, 0f64, 0f64, 0f64],
    [63f64, 0.1f64, -33f64, 0f64],
    [-59f64, 0f64, 26f64, 0f64],
    [-58f64, -0.1f64, 32f64, 0f64],
    [-51f64, 0f64, 27f64, 0f64],
    [48f64, 0f64, 0f64, 0f64],
    [46f64, 0f64, -24f64, 0f64],
    [-38f64, 0f64, 16f64, 0f64],
    [-31f64, 0f64, 13f64, 0f64],
    [29f64, 0f64, 0f64, 0f64],
    [29f64, 0f64, -12f64, 0f64],
    [26f64, 0f64, 0f64, 0f64],
    [-22f64, 0f64, 0f64, 0f64],
    [21f64, 0f64, -10f64, 0f64],
    [17f64, -0.1f64, 0f64, 0f64],
    [16f64, 0f64, -8f64, 0f64],
    [-16f64, 0.1f64, 7f64, 0f64],
    [-15f64, 0f64, 9f64, 0f64],
    [-13f64, 0f64, 7f64, 0f64],
    [-12f64, 0f64, 6f64, 0f64],
    [11f64, 0f64, 0f64, 0f64],
    [-10f64, 0f64, 5f64, 0f64],
    [-8f64, 0f64, 3f64, 0f64],
    [7f64, 0f64, -3f64, 0f64],
    [-7f64, 0f64, 0f64, 0f64],
    [-7f64, 0f64, 3f64, 0f64],
    [-7f64, 0f64, 3f64, 0f64],
    [6f64, 0f64, 0f64, 0f64],
    [6f64, 0f64, -3f64, 0f64],
    [6f64, 0f64, -3f64, 0f64],
    [-6f64, 0f64, 3f64, 0f64],
    [-6f64, 0f64, 3f64, 0f64],
    [5f64, 0f64, 0f64, 0f64],
    [-5f64, 0f64, 3f64, 0f64],
    [-5f64, 0f64, 3f64, 0f64],
    [-5f64, 0f64, 3f64, 0f64],
    [4f64, 0f64, 0f64, 0f64],
    [4f64, 0f64, 0f64, 0f64],
    [4f64, 0f64, 0f64, 0f64],
    [-4f64, 0f64, 0f64, 0f64],
    [-4f64, 0f64, 0f64, 0f64],
    [-4f64, 0f64, 0f64, 0f64],
    [3f64, 0f64, 0f64, 0f64],
    [-3f64, 0f64, 0f64, 0f64],
    [-3f64, 0f64, 0f64, 0f64],
    [-3f64, 0f64, 0f64, 0f64],
    [-3f64, 0f64, 0f64, 0f64],
    [-3f64, 0f64, 0f64, 0f64],
    [-3f64, 0f64, 0f64, 0f64],
    [-3f64, 0f64, 0f64, 0f64],
];

fn rad2deg(radians: f64) -> f64 {
    180.0f64 / PI * radians
}

fn deg2rad(degrees: f64) -> f64 {
    PI / 180.0f64 * degrees
}

fn limit_degrees(mut degrees: f64) -> f64 {
    let mut limited: f64 = 0.;
    degrees /= 360.0f64;
    limited = 360.0f64 * (degrees - degrees.floor());
    if limited < 0f64 {
        limited += 360.0f64;
    }
    limited
}

fn third_order_polynomial(a: f64, b: f64, c: f64, d: f64, x: f64) -> f64 {
    ((a * x + b) * x + c) * x + d
}

fn validate_inputs(spa: &SpaData) -> Result<(), SpaError> {
    if spa.jd < 990575.50000f64 || spa.jd > 3912880.49999f64 {
        return Err(SpaError::UnsupportedDate);
    }
    if spa.pressure < 0f64 || spa.pressure > 5000f64 {
        return Err(SpaError::InvalidPressure);
    }
    if spa.temperature <= -273f64 || spa.temperature > 6000f64 {
        return Err(SpaError::InvalidTemperature);
    }
    let x = spa.delta_t;
    if x.abs() > 8000f64 {
        return Err(SpaError::InvalidDeltaT);
    }
    let x = spa.longitude;
    if x.abs() > 180f64 {
        return Err(SpaError::InvalidLongitude);
    }
    let x = spa.latitude;
    if x.abs() > 90f64 {
        return Err(SpaError::InvalidLatitude);
    }
    let x = spa.atmos_refract;
    if x.abs() > 5f64 {
        return Err(SpaError::InvalidAtmosRefract);
    }
    if spa.elevation < -6500000f64 {
        return Err(SpaError::InvalidElevation);
    }
    Ok(())
}

fn julian_century(jd: f64) -> f64 {
    (jd - 2451545.0f64) / 36525.0f64
}

fn julian_ephemeris_day(jd: f64, delta_t: f64) -> f64 {
    jd + delta_t / 86400.0f64
}

fn julian_ephemeris_century(jde: f64) -> f64 {
    (jde - 2451545.0f64) / 36525.0f64
}

fn julian_ephemeris_millennium(jce: f64) -> f64 {
    jce / 10.0f64
}

fn earth_periodic_term_summation(terms: &[[f64; 3]], count: i32, jme: f64) -> f64 {
    let mut i: i32 = 0;
    let mut sum: f64 = 0f64;
    i = 0;
    while i < count {
        let x = terms[i as usize][TERM_B as usize] + terms[i as usize][TERM_C as usize] * jme;
        sum += terms[i as usize][TERM_A as usize] * x.cos();
        i += 1;
    }
    sum
}

fn earth_values(term_sum: &[f64], count: i32, jme: f64) -> f64 {
    let mut i: i32 = 0;
    let mut sum: f64 = 0f64;
    i = 0;
    while i < count {
        let y = i as f64;
        sum += term_sum[i as usize] * jme.powf(y);
        i += 1;
    }
    sum /= 1.0e8f64;
    sum
}

fn earth_heliocentric_longitude(jme: f64) -> f64 {
    let mut sum: [f64; 6] = [0.; 6];
    let mut i: i32 = 0;
    i = 0;
    while i < 6 {
        sum[i as usize] =
            earth_periodic_term_summation(&(L_TERMS[i as usize]), L_SUBCOUNT[i as usize], jme);
        i += 1;
    }
    limit_degrees(rad2deg(earth_values(&sum, 6, jme)))
}

fn earth_heliocentric_latitude(jme: f64) -> f64 {
    let mut sum: [f64; 2] = [0.; 2];
    let mut i: i32 = 0;
    i = 0;
    while i < 2 {
        sum[i as usize] =
            earth_periodic_term_summation(&(B_TERMS[i as usize]), B_SUBCOUNT[i as usize], jme);
        i += 1;
    }
    rad2deg(earth_values(&sum, 2, jme))
}

fn earth_radius_vector(jme: f64) -> f64 {
    let mut sum: [f64; 5] = [0.; 5];
    let mut i: i32 = 0;
    i = 0;
    while i < 5 {
        sum[i as usize] =
            earth_periodic_term_summation(&(R_TERMS[i as usize]), R_SUBCOUNT[i as usize], jme);
        i += 1;
    }
    earth_values(&sum, 5, jme)
}

fn geocentric_longitude(l: f64) -> f64 {
    let mut theta: f64 = l + 180.0f64;
    if theta >= 360.0f64 {
        theta -= 360.0f64;
    }
    theta
}

fn geocentric_latitude(b: f64) -> f64 {
    -b
}

fn mean_elongation_moon_sun(jce: f64) -> f64 {
    third_order_polynomial(
        1.0f64 / 189474.0f64,
        -0.0019142f64,
        445267.11148f64,
        297.85036f64,
        jce,
    )
}

fn mean_anomaly_sun(jce: f64) -> f64 {
    third_order_polynomial(
        -1.0f64 / 300000.0f64,
        -0.0001603f64,
        35999.05034f64,
        357.52772f64,
        jce,
    )
}

fn mean_anomaly_moon(jce: f64) -> f64 {
    third_order_polynomial(
        1.0f64 / 56250.0f64,
        0.0086972f64,
        477198.867398f64,
        134.96298f64,
        jce,
    )
}

fn argument_latitude_moon(jce: f64) -> f64 {
    third_order_polynomial(
        1.0f64 / 327270.0f64,
        -0.0036825f64,
        483202.017538f64,
        93.27191f64,
        jce,
    )
}

fn ascending_longitude_moon(jce: f64) -> f64 {
    third_order_polynomial(
        1.0f64 / 450000.0f64,
        0.0020708f64,
        -1934.136261f64,
        125.04452f64,
        jce,
    )
}

fn xy_term_summation(i: i32, x: &[f64]) -> f64 {
    let mut j: i32 = 0;
    let mut sum: f64 = 0f64;
    j = 0;
    while j < TERM_X_COUNT as i32 {
        sum += x[j as usize] * Y_TERMS[i as usize][j as usize] as f64;
        j += 1;
    }
    sum
}

fn nutation_longitude_and_obliquity(jce: f64, x: &[f64], del_psi: &mut f64, del_epsilon: &mut f64) {
    let mut i: i32 = 0;
    let mut xy_term_sum: f64 = 0.;
    let mut sum_psi: f64 = 0f64;
    let mut sum_epsilon: f64 = 0f64;
    i = 0;
    while i < 63 {
        xy_term_sum = deg2rad(xy_term_summation(i, x));
        sum_psi += (PE_TERMS[i as usize][TERM_PSI_A as usize]
            + jce * PE_TERMS[i as usize][TERM_PSI_B as usize])
            * xy_term_sum.sin();
        sum_epsilon += (PE_TERMS[i as usize][TERM_EPS_C as usize]
            + jce * PE_TERMS[i as usize][TERM_EPS_D as usize])
            * xy_term_sum.cos();
        i += 1;
    }
    *del_psi = sum_psi / 36000000.0f64;
    *del_epsilon = sum_epsilon / 36000000.0f64;
}

fn ecliptic_mean_obliquity(jme: f64) -> f64 {
    let u: f64 = jme / 10.0f64;
    84381.448f64
        + u * (-4680.93f64
            + u * (-1.55f64
                + u * (1999.25f64
                    + u * (-51.38f64
                        + u * (-249.67f64
                            + u * (-39.05f64
                                + u * (7.12f64 + u * (27.87f64 + u * (5.79f64 + u * 2.45f64)))))))))
}

fn ecliptic_true_obliquity(delta_epsilon: f64, epsilon0: f64) -> f64 {
    delta_epsilon + epsilon0 / 3600.0f64
}

fn aberration_correction(r: f64) -> f64 {
    -20.4898f64 / (3600.0f64 * r)
}

fn apparent_sun_longitude(theta: f64, delta_psi: f64, delta_tau: f64) -> f64 {
    theta + delta_psi + delta_tau
}

fn greenwich_mean_sidereal_time(jd: f64, jc: f64) -> f64 {
    limit_degrees(
        280.46061837f64
            + 360.98564736629f64 * (jd - 2451545.0f64)
            + jc * jc * (0.000387933f64 - jc / 38710000.0f64),
    )
}

fn greenwich_sidereal_time(nu0: f64, delta_psi: f64, epsilon: f64) -> f64 {
    let x = deg2rad(epsilon);
    nu0 + delta_psi * x.cos()
}

fn geocentric_right_ascension(lamda: f64, epsilon: f64, beta: f64) -> f64 {
    let lamda_rad: f64 = deg2rad(lamda);
    let epsilon_rad: f64 = deg2rad(epsilon);
    let x1 = deg2rad(beta);
    let x = lamda_rad.sin() * epsilon_rad.cos() - x1.tan() * epsilon_rad.sin();
    let y = lamda_rad.cos();
    limit_degrees(rad2deg(x.atan2(y)))
}

fn geocentric_declination(beta: f64, epsilon: f64, lamda: f64) -> f64 {
    let beta_rad: f64 = deg2rad(beta);
    let epsilon_rad: f64 = deg2rad(epsilon);
    let x = deg2rad(lamda);
    rad2deg(
        (beta_rad.sin() * epsilon_rad.cos() + beta_rad.cos() * epsilon_rad.sin() * x.sin()).asin(),
    )
}

fn observer_hour_angle(nu: f64, longitude: f64, alpha_deg: f64) -> f64 {
    limit_degrees(nu + longitude - alpha_deg)
}

fn sun_equatorial_horizontal_parallax(r: f64) -> f64 {
    8.794f64 / (3600.0f64 * r)
}

fn right_ascension_parallax_and_topocentric_dec(
    latitude: f64,
    elevation: f64,
    xi: f64,
    h: f64,
    delta: f64,
    delta_alpha: &mut f64,
    delta_prime: &mut f64,
) {
    let mut delta_alpha_rad: f64 = 0.;
    let lat_rad: f64 = deg2rad(latitude);
    let xi_rad: f64 = deg2rad(xi);
    let h_rad: f64 = deg2rad(h);
    let delta_rad: f64 = deg2rad(delta);
    let x = 0.99664719f64 * lat_rad.tan();
    let u: f64 = x.atan();
    let y: f64 = 0.99664719f64 * u.sin() + elevation * lat_rad.sin() / 6378140.0f64;
    let x: f64 = u.cos() + elevation * lat_rad.cos() / 6378140.0f64;
    let x1 = -x * xi_rad.sin() * h_rad.sin();
    let y1 = delta_rad.cos() - x * xi_rad.sin() * h_rad.cos();
    delta_alpha_rad = x1.atan2(y1);
    let x1 = (delta_rad.sin() - y * xi_rad.sin()) * delta_alpha_rad.cos();
    let y1 = delta_rad.cos() - x * xi_rad.sin() * h_rad.cos();
    *delta_prime = rad2deg(x1.atan2(y1));
    *delta_alpha = rad2deg(delta_alpha_rad);
}

fn topocentric_local_hour_angle(h: f64, delta_alpha: f64) -> f64 {
    h - delta_alpha
}

fn topocentric_elevation_angle(latitude: f64, delta_prime: f64, h_prime: f64) -> f64 {
    let lat_rad: f64 = deg2rad(latitude);
    let delta_prime_rad: f64 = deg2rad(delta_prime);
    let x = deg2rad(h_prime);
    rad2deg(
        (lat_rad.sin() * delta_prime_rad.sin() + lat_rad.cos() * delta_prime_rad.cos() * x.cos())
            .asin(),
    )
}

fn atmospheric_refraction_correction(
    pressure: f64,
    temperature: f64,
    atmos_refract: f64,
    e0: f64,
) -> f64 {
    let mut del_e: f64 = 0f64;
    if e0 >= -1f64 * (0.26667f64 + atmos_refract) {
        let x = deg2rad(e0 + 10.3f64 / (e0 + 5.11f64));
        del_e = pressure / 1010.0f64 * (283.0f64 / (273.0f64 + temperature)) * 1.02f64
            / (60.0f64 * x.tan());
    }
    del_e
}

fn topocentric_elevation_angle_corrected(e0: f64, delta_e: f64) -> f64 {
    e0 + delta_e
}

fn calculate_geocentric_sun_right_ascension_and_declination(spa: &mut SpaData) {
    let mut x: [f64; 5] = [0.; 5];
    spa.jc = julian_century(spa.jd);
    spa.jde = julian_ephemeris_day(spa.jd, spa.delta_t);
    spa.jce = julian_ephemeris_century(spa.jde);
    spa.jme = julian_ephemeris_millennium(spa.jce);
    spa.l = earth_heliocentric_longitude(spa.jme);
    spa.b = earth_heliocentric_latitude(spa.jme);
    spa.r = earth_radius_vector(spa.jme);
    spa.theta = geocentric_longitude(spa.l);
    spa.beta = geocentric_latitude(spa.b);
    spa.x0 = mean_elongation_moon_sun(spa.jce);
    x[TERM_X0 as usize] = spa.x0;
    spa.x1 = mean_anomaly_sun(spa.jce);
    x[TERM_X1 as usize] = spa.x1;
    spa.x2 = mean_anomaly_moon(spa.jce);
    x[TERM_X2 as usize] = spa.x2;
    spa.x3 = argument_latitude_moon(spa.jce);
    x[TERM_X3 as usize] = spa.x3;
    spa.x4 = ascending_longitude_moon(spa.jce);
    x[TERM_X4 as usize] = spa.x4;
    nutation_longitude_and_obliquity(spa.jce, &x, &mut spa.del_psi, &mut spa.del_epsilon);
    spa.epsilon0 = ecliptic_mean_obliquity(spa.jme);
    spa.epsilon = ecliptic_true_obliquity(spa.del_epsilon, spa.epsilon0);
    spa.del_tau = aberration_correction(spa.r);
    spa.lamda = apparent_sun_longitude(spa.theta, spa.del_psi, spa.del_tau);
    spa.nu0 = greenwich_mean_sidereal_time(spa.jd, spa.jc);
    spa.nu = greenwich_sidereal_time(spa.nu0, spa.del_psi, spa.epsilon);
    spa.alpha = geocentric_right_ascension(spa.lamda, spa.epsilon, spa.beta);
    spa.delta = geocentric_declination(spa.beta, spa.epsilon, spa.lamda);
}

impl SpaData {
    /// Calculate the Solar Position values for the given input parameters
    pub fn calculate(&mut self) -> Result<(), SpaError> {
        validate_inputs(&*self)?;
        calculate_geocentric_sun_right_ascension_and_declination(&mut *self);
        self.h = observer_hour_angle(self.nu, self.longitude, self.alpha);
        self.xi = sun_equatorial_horizontal_parallax(self.r);
        right_ascension_parallax_and_topocentric_dec(
            self.latitude,
            self.elevation,
            self.xi,
            self.h,
            self.delta,
            &mut self.del_alpha,
            &mut self.delta_prime,
        );
        self.h_prime = topocentric_local_hour_angle(self.h, self.del_alpha);
        self.e0 = topocentric_elevation_angle(self.latitude, self.delta_prime, self.h_prime);
        self.del_e = atmospheric_refraction_correction(
            self.pressure,
            self.temperature,
            self.atmos_refract,
            self.e0,
        );
        self.e = topocentric_elevation_angle_corrected(self.e0, self.del_e);
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn test_double(expected: f64, actual: f64) -> bool {
        let error = (expected - actual).abs() / expected;
        return error < 1.0e-5f64;
    }

    #[test]
    fn test() {
        let mut spa = SpaData::default();
        spa.jd = 2452930.312847f64;
        spa.delta_t = 67f64;
        spa.longitude = -105.1786f64;
        spa.latitude = 39.742476f64;
        spa.elevation = 1830.14f64;
        spa.pressure = 820 as i32 as f64;
        spa.temperature = 11 as i32 as f64;
        spa.atmos_refract = 0.5667f64;

        spa.calculate().unwrap();

        println!("Julian Day:    {:.6}", spa.jd);
        println!("L:             {:.6e}", spa.l);
        println!("B:             {:.6e}", spa.b);
        println!("R:             {:.6}", spa.r);
        println!("H:             {:.6}", spa.h);
        println!("Delta Psi:     {:.6e}", spa.del_psi);
        println!("Delta Epsilon: {:.6e}", spa.del_epsilon);
        println!("Epsilon:       {:.6}", spa.epsilon);

        assert!(test_double(2.401826e+01, spa.l));
        assert!(test_double(-1.011219e-04, spa.b));
        assert!(test_double(0.996542, spa.r));
        assert!(test_double(11.105902, spa.h));
        assert!(test_double(-3.998404e-03, spa.del_psi));
        assert!(test_double(1.666568e-03, spa.del_epsilon));
        assert!(test_double(23.440465, spa.epsilon));
    }
}
