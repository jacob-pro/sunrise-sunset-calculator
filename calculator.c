//
//  calculator.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 18/11/2018.
//
#include "calculator.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
	SolarEventSunrise,
	SolarEventSunset,
} SolarEvent;

double radToDeg(double angleRad) {
	return (180.0 * angleRad / M_PI);
}

double degToRad(double angleDeg) {
	return (M_PI * angleDeg / 180.0);
}

bool isGregorianLeapYear(int yr) {
	return ((yr % 4 == 0 && yr % 100 != 0) || yr % 400 == 0);
}

// AA - Page 65 - Day of the Year
int calcDayOfYear(int mn, int dy, bool lpyr) {
	int k = lpyr ? 1 : 2;
	int n = floor((275 * mn) / 9) - k * floor((mn + 9) / 12) + dy - 30;
	return n;
}

// AA - Page 59 - Calulation of the JD
double calcJDfromGregorianDate(int Y, int M, int D) {
	if (M <= 2) {
		Y -= 1;
		M += 12;
	}
	int A = floor(Y / 100);
	int B = 2 - A + floor(A / 4);
	double JD = floor(365.25*(Y + 4716)) + floor(30.6001*(M + 1)) + D + B - 1524.5;
	return JD;
}

double calcTimeJulianCent(double jd) {
	double T = (jd - 2451545.0) / 36525.0;
	return T;
}

double calcJDFromJulianCent(double t) {
	double JD = t * 36525.0 + 2451545.0;
	return JD;
}

double calcGeomMeanLongSun(double t) {
	double L0 = 280.46646 + t * (36000.76983 + 0.0003032 * t);
	while (L0 > 360.0) L0 -= 360.0;
	while (L0 < 0.0) L0 += 360.0;
	return L0;		// in degrees
}

double calcGeomMeanAnomalySun(double t) {
	double M = 357.52911 + t * (35999.05029 - 0.0001537 * t);
	return M;		// in degrees
}

double calcEccentricityEarthOrbit(double t) {
	double e = 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
	return e;
}

double calcSunEqOfCenter(double t) {
	double m = calcGeomMeanAnomalySun(t);
	double mrad = degToRad(m);
	double sinm = sin(mrad);
	double sin2m = sin(mrad + mrad);
	double sin3m = sin(mrad + mrad + mrad);
	double C = sinm * (1.914602 - t * (0.004817 + 0.000014 * t)) + sin2m * (0.019993 - 0.000101 * t) + sin3m * 0.000289;
	return C;		// in degrees
}

double calcSunTrueLong(double t) {
	double l0 = calcGeomMeanLongSun(t);
	double c = calcSunEqOfCenter(t);
	double O = l0 + c;
	return O;		// in degrees
}

double calcSunApparentLong(double t) {
	double o = calcSunTrueLong(t);
	double omega = 125.04 - 1934.136 * t;
	double lambda = o - 0.00569 - 0.00478 * sin(degToRad(omega));
	return lambda;		// in degrees
}

double calcMeanObliquityOfEcliptic(double t) {
	double seconds = 21.448 - t * (46.8150 + t * (0.00059 - t * (0.001813)));
	double e0 = 23.0 + (26.0 + (seconds / 60.0)) / 60.0;
	return e0;		// in degrees
}

double calcObliquityCorrection(double t) {
	double e0 = calcMeanObliquityOfEcliptic(t);
	double omega = 125.04 - 1934.136 * t;
	double e = e0 + 0.00256 * cos(degToRad(omega));
	return e;		// in degrees
}

double calcSunDeclination(double t) {
	double e = calcObliquityCorrection(t);
	double lambda = calcSunApparentLong(t);
	double sint = sin(degToRad(e)) * sin(degToRad(lambda));
	double theta = radToDeg(asin(sint));
	return theta;		// in degrees
}

double calcEquationOfTime(double t) {
	double epsilon = calcObliquityCorrection(t);
	double l0 = calcGeomMeanLongSun(t);
	double e = calcEccentricityEarthOrbit(t);
	double m = calcGeomMeanAnomalySun(t);

	double y = tan(degToRad(epsilon) / 2.0);
	y *= y;

	double sin2l0 = sin(2.0 * degToRad(l0));
	double sinm = sin(degToRad(m));
	double cos2l0 = cos(2.0 * degToRad(l0));
	double sin4l0 = sin(4.0 * degToRad(l0));
	double sin2m = sin(2.0 * degToRad(m));

	double Etime = y * sin2l0 - 2.0 * e * sinm + 4.0 * e * y * sinm * cos2l0
		- 0.5 * y * y * sin4l0 - 1.25 * e * e * sin2m;

	return radToDeg(Etime)*4.0;	// in minutes of time
}

double calcHourAngle(SolarEvent event, double lat, double solarDec) {
	double latRad = degToRad(lat);
	double sdRad = degToRad(solarDec);
	double HA = (acos(cos(degToRad(90.833)) / (cos(latRad)*cos(sdRad)) - tan(latRad) * tan(sdRad)));
	if (event == SolarEventSunset) HA = -HA;
	return HA;		// in radians
}

double calcSolNoonUTC(double t, double longitude) {
	// First pass uses approximate solar noon to calculate eqtime
	double tnoon = calcTimeJulianCent(calcJDFromJulianCent(t) - longitude / 360.0);
	double eqTime = calcEquationOfTime(tnoon);
	double solNoonUTC = 720 - (longitude * 4) - eqTime; // min

	double newt = calcTimeJulianCent(calcJDFromJulianCent(t) - 0.5 + solNoonUTC / 1440.0);

	eqTime = calcEquationOfTime(newt);
	solNoonUTC = 720 - (longitude * 4) - eqTime; // min

	return solNoonUTC;
}

double calcSunriseSetUTC(SolarEvent event, double JD, double latitude, double longitude) {
	double t = calcTimeJulianCent(JD);

	// *** Find the time of solar noon at the location, and use
	//     that declination. This is better than start of the 
	//     Julian day

	double noonmin = calcSolNoonUTC(t, longitude);
	double tnoon = calcTimeJulianCent(JD + noonmin / 1440.0);

	// *** First pass to approximate sunrise (using solar noon)

	double eqTime = calcEquationOfTime(tnoon);
	double solarDec = calcSunDeclination(tnoon);
	double hourAngle = calcHourAngle(event, latitude, solarDec);

	double delta = longitude + radToDeg(hourAngle);
	double timeDiff = 4 * delta;	// in minutes of time
	double timeUTC = 720 - timeDiff - eqTime;	// in minutes

	// *** Second pass includes fractional jday in gamma calc

	double newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC / 1440.0);
	eqTime = calcEquationOfTime(newt);
	solarDec = calcSunDeclination(newt);
	hourAngle = calcHourAngle(event, latitude, solarDec);
	delta = longitude + radToDeg(hourAngle);
	timeDiff = 4 * delta;
	timeUTC = 720 - timeDiff - eqTime; // in minutes

	return timeUTC;
}

double calcJDofNearestRiseSet(bool forwards, SolarEvent event, double JD, double latitude, double longitude) {
	double julianday = JD;
	double increment = (forwards == true) ? 1.0 : -1.0;
	double time = calcSunriseSetUTC(event, julianday, latitude, longitude);
	while (isnan(time)) {
		julianday += increment;
		time = calcSunriseSetUTC(event, julianday, latitude, longitude);
	}
	return julianday;
}

//// AA - Page 63 - Calculation of the Calendar Date from the JD
//void calcGregorianDateFromJD(double jd, int *dayPtr, int *monthPtr, int *yearPtr) {
//	jd += 0.5;
//	int Z = floor(jd);
//	double F = jd - Z;
//
//	int A;
//	//if (Z < 2299161) {		//I disabled this for proleptic Gregorian, however it still breaks for old dates
//	//	A = Z;
//	//} else {
//		int alpha = floor((Z - 1867216.25) / 36524.25);
//		A = Z + 1 + alpha - floor(alpha / 4);
//	//}
//
//	int B = A + 1524;
//	int C = floor((B - 122.1) / 365.25);
//	int D = floor(365.25 * C);
//	int E = floor((B - D) / 30.6001);
//
//	int day = B - D - floor(30.6001 * E) + F;
//	int month = (E < 14) ? E - 1 : E - 13;
//	int year = (month > 2) ? C - 4716 : C - 4715;
//
//	*dayPtr = day;
//	*monthPtr = month;
//	*yearPtr = year;
//}

// From https://quasar.as.utexas.edu/BillInfo/JulianDatesG.html
// It apparently isn't supposed to work prior to 400, yet it does
void calcGregorianDateFromJD(double JD, int *dayPtr, int *monthPtr, int *yearPtr) {
	double Q = JD + 0.5;
	int Z = floor(Q);
	int W = (Z - 1867216.25) / 36524.25;
	int X = W / 4;
	int A = Z + 1 + W - X;
	int B = A + 1524;
	int C = (B - 122.1) / 365.25;
	int D = 365.25 * C;
	int E = (B - D) / 30.6001;
	int F = 30.6001 * E;
	int day = B - D - F + (Q - Z);
	int month = (E < 14) ? E - 1 : E - 13;
	int year = (month <= 2) ? (C - 4715) : (C - 4716);

	*dayPtr = day;
	*monthPtr = month;
	*yearPtr = year;
}

// Convert the minutes of sunrise/sunset for a JD to a calendar date and time
void populate_date_time(double minutes, double JD, SSDateTime *date) {
	double julianday = JD;
	double floatHour = minutes / 60.0;
	int hour = floor(floatHour);
	double floatMinute = 60.0 * (floatHour - hour);
	int minute = floor(floatMinute);
	double floatSec = 60.0 * (floatMinute - minute);
	int second = floor(floatSec + 0.5);

	minute += (second >= 30) ? 1 : 0;

	if (minute >= 60) {
		minute -= 60;
		hour++;
	}

	if (hour > 23) {
		hour -= 24;
		julianday += 1.0;
	}

	if (hour < 0) {
		hour += 24;
		julianday -= 1.0;
	}

	date->hours = hour;
	date->mins = minute;
	calcGregorianDateFromJD(julianday, &date->day, &date->month, &date->year);
}

void calculate_time_for_event(SolarEvent event, double JD, int doy, double latitude, double longitude, SSDateTime *result) {

	double riseTimeGMT = calcSunriseSetUTC(event, JD, latitude, longitude);
	if (!isnan(riseTimeGMT)) {
		populate_date_time(riseTimeGMT, JD, result);
	} else {
		double newjd, newtime;
		bool x = (event == SolarEventSunset);
		if (((latitude > 66.4) && (doy > 79) && (doy < 267)) ||	((latitude < -66.4) && ((doy < 83) || (doy > 263)))) {
			newjd = calcJDofNearestRiseSet(x, event, JD, latitude, longitude);
			newtime = calcSunriseSetUTC(event, newjd, latitude, longitude);
		} else {
			newjd = calcJDofNearestRiseSet(!x, event, JD, latitude, longitude);
			newtime = calcSunriseSetUTC(event, newjd, latitude, longitude);
		}
		populate_date_time(newtime, newjd, result);
	}
}

// Check the latitude and longitude values are valid
bool validate_coordinates(double latitude, double longitude) {
	if (latitude < -90 || latitude > 90) return false;
	if (longitude < -180 || longitude > 180) return false;
	return true;
}

bool validate_year_range(int year) {
	return (year >= -1000 && year <= 3000);
}

// Check that the date is valid
bool validate_gregorian_date(int year, int month, int day) {
	if (month < 1 || month > 12) return false;
	if (day < 1) return false;
	static int const monthLengths[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	bool leapYear = isGregorianLeapYear(year);
	if ((!leapYear || month != 2) && day > monthLengths[month - 1]) return false;
	if (month == 2 && leapYear && day > 29) return false;
	return true;
}

SSCalculationStatus CalculateSunriseSunset(int year, int month, int day, double latitude, double longitude, SSDateTime *rise, SSDateTime *set) {

	if (!validate_coordinates(latitude, longitude)) return SSCalculationStatusInvalidCoords;
	if (!validate_gregorian_date(year, month, day)) return SSCalculationStatusInvalidDate;
	if (!validate_year_range(year)) return SSCalculationStatusYearUnsupported;

	if (latitude < -89) latitude = -89;		//All latitudes between 89 and 90 S will be set to -89
	if (latitude > 89) latitude = 89;		//All latitudes between 89 and 90 N will be set to 89

	double JD = calcJDfromGregorianDate(year, month, day);
	int doy = calcDayOfYear(month, day, isGregorianLeapYear(year));

	calculate_time_for_event(SolarEventSunrise, JD, doy, latitude, longitude, rise);
	calculate_time_for_event(SolarEventSunset, JD, doy, latitude, longitude, set);

	return SSCalculationStatusSuccess;
}


#ifdef TEST_CALCULATOR

#include "tinytest/tinytest.h"

static SSDateTime rise, set;
static int year, month, day;

#define ASSERT_DATE_TIME_EQUAL(Y, Mo, D, H, Mi, dt) \
	ASSERT_EQUALS(Y, dt.year); \
	ASSERT_EQUALS(Mo, dt.month); \
	ASSERT_EQUALS(D, dt.day); \
	ASSERT_EQUALS(H, dt.hours); \
	ASSERT_EQUALS(Mi, dt.mins); \

#define ASSERT_DATE_EQUAL(Y, M, D) \
	ASSERT_EQUALS(Y, year); \
	ASSERT_EQUALS(M, month); \
	ASSERT_EQUALS(D, day); \

// Some basic checks that the results match the web version
#define BRISTOL_LAT 51.4545
#define BRISTOL_LON -2.5879

void test_bristol() {
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2018, 11, 18, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(2018, 11, 18, 7, 33, rise);		//Test 2018 Winter
    ASSERT_DATE_TIME_EQUAL(2018, 11, 18, 16, 17, set);
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2018, 6, 1, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(2018, 6, 1, 3, 59, rise);		//Test 2018 Summer
    ASSERT_DATE_TIME_EQUAL(2018, 6, 1, 20, 18, set);
}

// Test a significantly different time zone to UTC to check that we don't get any negatives back
#define TOKYO_LAT 35.7
#define TOKYO_LON 139.77
#define VANCOUVER_LAT 49.267
#define VANCOUVER_LON -123.117

void test_different_timezones() {
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2020, 2, 15, TOKYO_LAT, TOKYO_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(2020, 2, 14, 21, 29, rise);
    ASSERT_DATE_TIME_EQUAL(2020, 2, 15, 8, 22, set);
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2010, 9, 1, TOKYO_LAT, TOKYO_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(2010, 8, 31, 20, 12, rise);
    ASSERT_DATE_TIME_EQUAL(2010, 9, 1, 9, 9, set);
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(1980, 5, 31, VANCOUVER_LAT, VANCOUVER_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(1980, 5, 31, 12, 12, rise);
    ASSERT_DATE_TIME_EQUAL(1980, 6, 1, 4, 9, set);
}

// Test some ancient dates

void test_old_dates() {
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(1582, 10, 15, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(1582, 10, 15, 6, 34, rise);
    ASSERT_DATE_TIME_EQUAL(1582, 10, 15, 17, 18, set);

    // I cannot actually test dates prior to 15/10/1582 because the original NOAA calculator is broken so I have nothing to compare the times too.
    // Instead I will test the part of the calculator which was broken and that I have now (hopefully) fixed
    // Results are sourced from https://www.fourmilab.ch/documents/calendar/ and https://c14.arch.ox.ac.uk/oxcalhelp/hlp_analysis_calend.html
    // All dates are proleptic Gregorian as per the program specification

    // 14/10/1582
    ASSERT_EQUALS(2299159.5, calcJDfromGregorianDate(1582, 10, 14));
    calcGregorianDateFromJD(2299159.5, &day, &month, &year);
    ASSERT_DATE_EQUAL(1582, 10, 14);

    // 1/1/400
    ASSERT_EQUALS(1867156.5, calcJDfromGregorianDate(400, 1, 1));
    calcGregorianDateFromJD(1867156.5, &day, &month, &year);
    ASSERT_DATE_EQUAL(400, 1, 1);

    // 31/12/399
    ASSERT_EQUALS(1867155.5, calcJDfromGregorianDate(399, 12, 31));
    calcGregorianDateFromJD(1867155.5, &day, &month, &year);
    ASSERT_DATE_EQUAL(399, 12, 31);

    // 1/3/0
    ASSERT_EQUALS(1721119.5, calcJDfromGregorianDate(0, 3, 1));
    calcGregorianDateFromJD(1721119.5, &day, &month, &year);
    ASSERT_DATE_EQUAL(0, 3, 1);

    // 29/2/0		(A leap year on proleptic Gregorian)
    ASSERT_EQUALS(1721118.5, calcJDfromGregorianDate(0, 2, 29));
    calcGregorianDateFromJD(1721118.5, &day, &month, &year);
    ASSERT_DATE_EQUAL(0, 2, 29);

    // 1/1/-500
    ASSERT_EQUALS(1538438.5, calcJDfromGregorianDate(-500, 1, 1));
    calcGregorianDateFromJD(1538438.5, &day, &month, &year);
    ASSERT_DATE_EQUAL(-500, 1, 1);
}

// Check that the extreme latitude cases match the web version
// Northern hemisphere
#define SVALBARD_LAT 79.0
#define SVALBARD_LON 17.0
//Southern hemisphere
#define MCMURDO_LAT -77.8499966
#define MCMURDO_LON 166.666664

void test_big_latitudes() {
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2018, 7, 1, SVALBARD_LAT, SVALBARD_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(2018, 4, 15, 0, 2, rise);		//The the Svalbard Summer
    ASSERT_DATE_TIME_EQUAL(2018, 8, 27, 21, 56, set);
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2018, 10, 31, SVALBARD_LAT, SVALBARD_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(2018, 10, 24, 10, 51, set);		//The the Svalbard Winter
    ASSERT_DATE_TIME_EQUAL(2019, 2, 18, 10, 25, rise);
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(1950, 5, 1, MCMURDO_LAT, MCMURDO_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(1950, 4, 25, 1, 9, set);		//The the Antarctic Winter
    ASSERT_DATE_TIME_EQUAL(1950, 8, 20, 0, 8, rise);
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(1950, 12, 10, MCMURDO_LAT, MCMURDO_LON, &rise, &set));
    ASSERT_DATE_TIME_EQUAL(1950, 10, 22, 13, 33, rise);		//The the Antarctic Summer
    ASSERT_DATE_TIME_EQUAL(1951, 2, 20, 12, 23, set);
}

// Check that the validation functions are working

void test_validation_calendar() {
    //Number of months
    ASSERT_EQUALS(SSCalculationStatusInvalidDate, CalculateSunriseSunset(2018, 0, 1, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2018, 6, 1, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusInvalidDate, CalculateSunriseSunset(2018, 13, 1, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    //Month length
    ASSERT_EQUALS(SSCalculationStatusInvalidDate, CalculateSunriseSunset(2018, 1, 0, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    //Jan Length
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2018, 1, 31, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusInvalidDate, CalculateSunriseSunset(2018, 1, 32, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    //Nov Length
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2018, 11, 30, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusInvalidDate, CalculateSunriseSunset(2018, 11, 31, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    //February length
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2016, 2, 29, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusInvalidDate, CalculateSunriseSunset(2018, 2, 29, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    //Year validity
    ASSERT_EQUALS(SSCalculationStatusYearUnsupported, CalculateSunriseSunset(-1001, 1, 1, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(1000, 1, 1, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusYearUnsupported, CalculateSunriseSunset(3001, 1, 1, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(3000, 1, 1, BRISTOL_LAT, BRISTOL_LON, &rise, &set));
}

void test_validation_coords() {
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2018, 6, 1, 90, 180, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusSuccess, CalculateSunriseSunset(2018, 6, 1, -90, -180, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusInvalidCoords, CalculateSunriseSunset(2018, 6, 1, 91, 181, &rise, &set));
    ASSERT_EQUALS(SSCalculationStatusInvalidCoords, CalculateSunriseSunset(2018, 6, 1, -91, -181, &rise, &set));
}

int main() {
    RUN(test_bristol);
    RUN(test_different_timezones);
    RUN(test_old_dates);
    RUN(test_big_latitudes);
    RUN(test_validation_calendar);
    RUN(test_validation_coords);
    return TEST_REPORT();
}

#endif
