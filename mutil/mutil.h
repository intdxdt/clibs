#include <cmath>

#ifndef MUTIL_MUTIL_H
#define MUTIL_MUTIL_H

const double PRECISION = 12;
const double EPSILON = 1.0e-12;
const double Ln2 = M_LN2;
const double Sqrt2 = M_SQRT2;
const double Pi = M_PI;
const double Tau = 2.0 * Pi;

bool feq(double a, double b, double eps = EPSILON) {
    return (a == b) || (fabs(a - b) < eps);
}

#endif //MUTIL_MUTIL_H
