//
//	gmtime.h
//	Created by Jacob Halsey on 19/11/2018.
//
//	Unix gm time functions on Windows
//
#ifndef GMTIME_H
#define GMTIME_H

#include <time.h>

#ifdef WIN32
#define timegm _mkgmtime
#define gmtime_r(time, result) gmtime_s(result, time)
#endif

#endif
