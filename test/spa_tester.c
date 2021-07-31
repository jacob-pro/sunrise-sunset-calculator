
/////////////////////////////////////////////
//          SPA TESTER for SPA.C           //
//                                         //
//      Solar Position Algorithm (SPA)     //
//                   for                   //
//        Solar Radiation Application      //
//                                         //
//             August 12, 2004             //
//                                         //
//   Filename: SPA_TESTER.C                //
//                                         //
//   Afshin Michael Andreas                //
//   afshin_andreas@nrel.gov (303)384-6383 //
//                                         //
//   Measurement & Instrumentation Team    //
//   Solar Radiation Research Laboratory   //
//   National Renewable Energy Laboratory  //
//   1617 Cole Blvd, Golden, CO 80401      //
/////////////////////////////////////////////

// File modified by Jacob Halsey for use in sunrise-sunset-calculator

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "spa.h"

static bool test_double(double expected, double actual) {
    double error = fabs(expected - actual) / expected;
    return error < 1.0e-5;
}

int main()
{
    spa_data spa;
    spa.jd            = 2452930.312847;  /* 2003-10-17 12:30:30 (-7) */
    spa.delta_ut1     = 0;
    spa.delta_t       = 67;
    spa.longitude     = -105.1786;
    spa.latitude      = 39.742476;
    spa.elevation     = 1830.14;
    spa.pressure      = 820;
    spa.temperature   = 11;
    spa.atmos_refract = 0.5667;

    int result = spa_calculate(&spa);
    assert(result == 0);

    printf("Julian Day:    %.6f\n",spa.jd);
    printf("L:             %.6e degrees\n",spa.l);
    printf("B:             %.6e degrees\n",spa.b);
    printf("R:             %.6f AU\n",spa.r);
    printf("H:             %.6f degrees\n",spa.h);
    printf("Delta Psi:     %.6e degrees\n",spa.del_psi);
    printf("Delta Epsilon: %.6e degrees\n",spa.del_epsilon);
    printf("Epsilon:       %.6f degrees\n",spa.epsilon);

    assert(test_double(2.401826e+01, spa.l));
    assert(test_double(-1.011219e-04, spa.b));
    assert(test_double(0.996542, spa.r));
    assert(test_double(11.105902, spa.h));
    assert(test_double(-3.998404e-03, spa.del_psi));
    assert(test_double(1.666568e-03, spa.del_epsilon));
    assert(test_double(23.440465, spa.epsilon));

    return 0;
}

/////////////////////////////////////////////
// The output of this program should be:
//
//Julian Day:    2452930.312847
//L:             2.401826e+01 degrees
//B:             -1.011219e-04 degrees
//R:             0.996542 AU
//H:             11.105902 degrees
//Delta Psi:     -3.998404e-03 degrees
//Delta Epsilon: 1.666568e-03 degrees
//Epsilon:       23.440465 degrees
//
/////////////////////////////////////////////
