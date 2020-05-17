# Sunrise Sunset Calculator

A C library for working with Sunrise and Sunset times. 

`calculator.h` is a port of the Javascript algorithm created by the US National Oceanic and Atmospheric Administration (NOAA), found here: https://www.esrl.noaa.gov/gmd/grad/solcalc/sunrise.html Their calculator is in turn based on equations from Astronomical Algorithms, by Jean Meeus. 

The main advantage of this particular implementation is that it doesn't break at very high/low longitudes (e.g. Svalbard, Norway has 60 days of Midnight Sun). If there was no sunrise or sunset on the given day it will search for the previous/next date and time.

`adapter.h` is a wrapper around the calculator that finds for a given point in time if is day or night, and then the next/previous sunset/rise respectively.

`brightness.h` is an algorithm for calculating monitor brightness given a sunrise and sunset result. (See `brightness.png`)

#### Calendar bug in NOAA Algorithm

The NOAA algorithm is supposed to be valid for years -1000 to 3000 CE. 
According to its specification all dates are supposed to be on the Gregorian calendar (known as the proleptic Gregorian calendar when it is extended prior to the epoch). 

However I discovered that the calculator (and the new version as well) were producing strange results for dates prior to 15 Oct 1582.  
This is because while the function `calcJD` is always using the Gregorian calendar, the function that converts from a JD back to a calendar date  (`calcDayFromJD`) is changing the calculation on the boundary of Julian day 2299161 (15 Oct 1582) to the Julian calendar, thereby making the conversions inconsistent.

#### Changes made in my implementation

- Fixed the Gregorian calendar issues:
  - The function to convert a JD to calendar date has been completely replaced (`calcGregorianDateFromJD`)
  - Functions that deal with Gregorian calendar dates are more clearly named
- Longitude has been restored to the standard definition where East is positive
- Duplicate code in 2 hour angle calculations moved into 1 function (`calcHourAngle`)
- Duplicate code in 2 sunrise and sunset functions has been moved into 1 function (`calcSunriseSetUTC`)
- Duplicate code in 4 functions to calculate the nearest/previous sunrise/sunset moved into 1 function (`calcJDofNearestRiseSet`)
- Duplicate code in main calculation moved into a new function (`calculate_time_for_event`)
- Removed support for different time zones; everything is UTC.
