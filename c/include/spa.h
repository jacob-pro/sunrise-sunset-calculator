//
//  spa.h
//  https://midcdmz.nrel.gov/spa/spa.h
//  Modified for use in sunrise-sunset-calculator
//

/////////////////////////////////////////////
//          HEADER FILE for SPA.C          //
//                                         //
//      Solar Position Algorithm (SPA)     //
//                   for                   //
//        Solar Radiation Application      //
//                                         //
//               May 12, 2003              //
//                                         //
//   Filename: SPA.H                       //
//                                         //
//   Afshin Michael Andreas                //
//   afshin_andreas@nrel.gov (303)384-6383 //
//                                         //
//   Measurement & Instrumentation Team    //
//   Solar Radiation Research Laboratory   //
//   National Renewable Energy Laboratory  //
//   1617 Cole Blvd, Golden, CO 80401      //
/////////////////////////////////////////////

#ifndef __solar_position_algorithm_header
#define __solar_position_algorithm_header

typedef enum {
    SpaError_Success = 0,
    SpaError_UnsupportedDate = 1,
    SpaError_InvalidPressure = 12,
    SpaError_InvalidTemperature = 13,
    SpaError_InvalidDeltaT = 7,
    SpaError_InvalidLongitude = 9,
    SpaError_InvalidLatitude = 10,
    SpaError_InvalidAtmosRefract = 16,
    SpaError_InvalidElevation = 11,
} SpaError;

typedef struct
{
    //----------------------INPUT VALUES------------------------

    double jd;           // Julian day


    double delta_t;      // Difference between earth rotation time and terrestrial time
                         // It is derived from observation only and is reported in this
                         // bulletin: http://maia.usno.navy.mil/ser7/ser7.dat,
                         // where delta_t = 32.184 + (TAI-UTC) - DUT1
                         // valid range: -8000 to 8000 seconds, error code: 7

    double longitude;    // Observer longitude (negative west of Greenwich)
                         // valid range: -180  to  180 degrees, error code: 9

    double latitude;     // Observer latitude (negative south of equator)
                         // valid range: -90   to   90 degrees, error code: 10

    double elevation;    // Observer elevation [meters]
                         // valid range: -6500000 or higher meters,    error code: 11

    double pressure;     // Annual average local pressure [millibars]
                         // valid range:    0 to 5000 millibars,       error code: 12

    double temperature;  // Annual average local temperature [degrees Celsius]
                         // valid range: -273 to 6000 degrees Celsius, error code; 13

    double atmos_refract;// Atmospheric refraction at sunrise and sunset (0.5667 deg is typical)
                         // valid range: -5   to   5 degrees, error code: 16

    //-----------------Intermediate OUTPUT VALUES--------------------

    double jc;          //Julian century

    double jde;         //Julian ephemeris day
    double jce;         //Julian ephemeris century
    double jme;         //Julian ephemeris millennium

    double l;           //earth heliocentric longitude [degrees]
    double b;           //earth heliocentric latitude [degrees]
    double r;           //earth radius vector [Astronomical Units, AU]

    double theta;       //geocentric longitude [degrees]
    double beta;        //geocentric latitude [degrees]

    double x0;          //mean elongation (moon-sun) [degrees]
    double x1;          //mean anomaly (sun) [degrees]
    double x2;          //mean anomaly (moon) [degrees]
    double x3;          //argument latitude (moon) [degrees]
    double x4;          //ascending longitude (moon) [degrees]

    double del_psi;     //nutation longitude [degrees]
    double del_epsilon; //nutation obliquity [degrees]
    double epsilon0;    //ecliptic mean obliquity [arc seconds]
    double epsilon;     //ecliptic true obliquity  [degrees]

    double del_tau;     //aberration correction [degrees]
    double lamda;       //apparent sun longitude [degrees]
    double nu0;         //Greenwich mean sidereal time [degrees]
    double nu;          //Greenwich sidereal time [degrees]

    double alpha;       //geocentric sun right ascension [degrees]
    double delta;       //geocentric sun declination [degrees]

    double h;           //observer hour angle [degrees]
    double xi;          //sun equatorial horizontal parallax [degrees]
    double del_alpha;   //sun right ascension parallax [degrees]
    double delta_prime; //topocentric sun declination [degrees]
    double h_prime;     //topocentric local hour angle [degrees]

    double e0;          //topocentric elevation angle (uncorrected) [degrees]
    double del_e;       //atmospheric refraction correction [degrees]

    //---------------------Final OUTPUT VALUES------------------------

    double e;           //topocentric elevation angle (corrected) [degrees]

} spa_data;

SpaError spa_calculate(spa_data *spa);

#endif
