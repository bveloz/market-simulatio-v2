#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mymath.h"
#ifndef M_PI
#define M_PI 3.14159
#endif

double random_bell_curve(double min, double max, double mean)
{
    double stdDev = (max - min) / 6.0;
    double u1 = rand() / (double)RAND_MAX;
    double u2 = rand() / (double)RAND_MAX;
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return mean + z0 * stdDev;
}