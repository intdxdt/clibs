//
// Created by titus on 01/01/18.
//
#ifndef MBR_UTIL_H
#define MBR_UTIL_H
#include <math.h>
#include <stdbool.h>

#define EPSILON  1.0e-12

double feq(double a, double b){
    return a == b || fabs(a - b) < EPSILON;
}

#endif //MBR_UTIL_H
