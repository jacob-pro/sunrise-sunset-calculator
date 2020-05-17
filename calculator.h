//
//  calculator.h
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 18/11/2018.
//
//  Based on the "NOAA Improved Sunrise/Sunset Calculation" Javascript algorithm
//  https://www.esrl.noaa.gov/gmd/grad/solcalc/sunrise.html
//
#ifndef CALCULATOR_H
#define CALCULATOR_H

 // Time is UTC
typedef struct {
	int year;
	int month;
	int day;
	int hours;
	int mins;
} SSDateTime;

typedef enum {
	SSCalculationStatusSuccess,
	SSCalculationStatusInvalidCoords,
	SSCalculationStatusInvalidDate,
	SSCalculationStatusYearUnsupported,
} SSCalculationStatus;

// Notes on accuracy:
// +- 1 mins where latitude <= 72
// +- 10 mins where latitude > 72
// Lattitudes between 89 and 90 will be treated as 89

// All dates are on the Gregorian calendar
// Years must be within the range of -1000 and +3000 C.E.
// Latitude is N, Longitude is E
// Confirm that SSCalculationStatusSuccess is returned before using dates.
SSCalculationStatus CalculateSunriseSunset(int year, int month, int day, double latitude, double longitude, SSDateTime *rise, SSDateTime *set);

#endif
