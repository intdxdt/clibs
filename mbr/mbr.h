//
// Created by titus on 27/12/17.
//
#ifndef MBR_MBR_H
#define MBR_MBR_H

#include "util.h"

typedef struct {
    double minx, miny;
    double maxx, maxy;
} mbr;

mbr new_mbr(double minx, double miny, double maxx, double maxy) {
    return (mbr) {
            fmin(minx, maxx), fmin(miny, maxy),
            fmax(minx, maxx), fmax(miny, maxy),
    };
}

//Compare equality of two bounding boxes
bool mbr_equals(mbr a, mbr b) {
    return (
            feq(a.maxx, b.maxx) &&
            feq(a.maxy, b.maxy) &&
            feq(a.minx, b.minx) &&
            feq(a.miny, b.miny));
}

#endif //MBR_MBR_H
