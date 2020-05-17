//
//  adapter.c
//  Sunrise Sunset Calculator
//  Created by Jacob Halsey on 18/11/2018.
//
#include "adapter.h"
#include "gmtime.h"

#define calculate_sunrise_sunset_tm(time,lat,lon,rise,set)  \
        CalculateSunriseSunset((time.tm_year + 1900), (time.tm_mon + 1), time.tm_mday, lat, lon, rise, set)

// Convert the UTC date and time result from the sunrise sunset calculator to a time_t
time_t ssdatetime_to_time_t(SSDateTime *date) {
	struct tm tmstr;
	tmstr.tm_year = date->year - 1900;
	tmstr.tm_mon = date->month - 1;
	tmstr.tm_mday = date->day;
	tmstr.tm_hour = date->hours;
	tmstr.tm_min = date->mins;
	tmstr.tm_sec = 0;
	return timegm(&tmstr);
}

// Increment/decrement the day of a struct tm 
void change_tm_date(struct tm *time, int amount) {
	time->tm_mday += amount;
	mktime(time);
}

#define CALCULATION() \
 status = calculate_sunrise_sunset_tm(atTimeTM, lat, lon, &rise, &set); \
 if(status != SSCalculationStatusSuccess) return status; \
 riset = ssdatetime_to_time_t(&rise); \
 sett = ssdatetime_to_time_t(&set);

SSCalculationStatus SunriseSunsetAroundTime(double lat, double lon, time_t atTime, SunriseSunsetResult *result) {
	result->time = atTime;
	struct tm atTimeTM;
	gmtime_r(&atTime, &atTimeTM);		//Convert the atTime to a struct UTC, for use in the calculator
	SSDateTime rise, set;
	time_t riset, sett;
	SSCalculationStatus status;
	CALCULATION();

	if (riset < sett) {

		if ((riset <= atTime) && (atTime <= sett)) {
			result->visible = true;
			result->rise = riset;
			result->set = sett;
		} else if (atTime < riset) {
			result->visible = false;
			result->rise = riset;
			//get yesterday set
			change_tm_date(&atTimeTM, -1);
			CALCULATION();
			result->set = sett;

		} else {		// Implies atTime > sett
			result->visible = false;
			result->set = sett;
			//get tommorow rise
			change_tm_date(&atTimeTM, +1);
			CALCULATION();
			result->rise = riset;

		}

	} else {	//Implies sett <= riset

		if ((sett <= atTime) && (atTime <= riset)) {
			result->visible = false;
			result->rise = riset;
			result->set = sett;
		} else if (atTime < sett) {
			result->visible = true;
			result->set = sett;
			//get yesterday rise
			change_tm_date(&atTimeTM, -1);
			CALCULATION();
			result->rise = riset;

		} else {		// Implies atTime > riset
			result->visible = true;
			result->rise = riset;
			//get tommorow set
			change_tm_date(&atTimeTM, +1);
			CALCULATION();
			result->set = sett;

		}
	}

	return SSCalculationStatusSuccess;
}

#ifdef TEST_ADAPTER

#include "tinytest/tinytest.h"
#include "time_util.h"

static SunriseSunsetResult result;

#define ASSERT_VALID_RESULT(r, s, v) \
	ASSERT_EQUALS(r, result.rise); \
	ASSERT_EQUALS(s, result.set); \
	ASSERT_EQUALS(v, result.visible); \

#define BRISTOL_LAT 51.4545
#define BRISTOL_LON -2.5879

void test_bristol() {
	//Early morning of Monday 18th, Sun is not visible. It set at 16:17 the day before, and is due to rise at 7:35
	time_t set18 = time_t_for_time(2018, 11, 18, 16, 17, 0);	// 16:17 Sunset
	time_t early19 = time_t_for_time(2018, 11, 19, 5, 30, 0);	// 5:30 Early morning
	time_t rise19 = time_t_for_time(2018, 11, 19, 7, 35, 0);		// 7:35 Sunrise
	ASSERT_EQUALS(SSCalculationStatusSuccess, SunriseSunsetAroundTime(BRISTOL_LAT, BRISTOL_LON, early19, &result));
	ASSERT_VALID_RESULT(rise19, set18, false);

	//In the middle of Monday 18th, Sun is visible. It should have risen at 7:35 and be due to set at 16:16
	time_t midday19 = time_t_for_time(2018, 11, 19, 12, 0, 0);		// 12:00 Midday
	time_t set19 = time_t_for_time(2018, 11, 19, 16, 16, 0);		// 16:16 Sunset
	ASSERT_EQUALS(SSCalculationStatusSuccess, SunriseSunsetAroundTime(BRISTOL_LAT, BRISTOL_LON, midday19, &result));
	ASSERT_VALID_RESULT(rise19, set19, true);

	//In the evening of Monday 18th, Sun is not visible. It should have set at 16:16 and be due to rise tomorrow at 7:36
	time_t late19 = time_t_for_time(2018, 11, 19, 20, 25, 30);	// 20:25 Evening
	time_t rise20 = time_t_for_time(2018, 11, 20, 7, 36, 0);		// 7:36 Sunrise
	ASSERT_EQUALS(SSCalculationStatusSuccess, SunriseSunsetAroundTime(BRISTOL_LAT, BRISTOL_LON, late19, &result));
	ASSERT_VALID_RESULT(rise20, set19, false);
}

#define SVALBARD_LAT 79.0
#define SVALBARD_LON 17.0

void test_svalbard() {
	//TODO: add this test
}

// TODO: Check some boundary conditions, I am not 100% sure what happens there

int main() {
	RUN(test_bristol);
	RUN(test_svalbard);
	return TEST_REPORT();
}

#endif
