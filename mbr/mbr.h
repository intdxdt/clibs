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

mbr NewMBR(double minx, double miny, double maxx, double maxy) {
    return (mbr) {
        min(minx, maxx), min(miny, maxy),
        max(minx, maxx), max(miny, maxy),
    };
}

#endif //MBR_MBR_H
