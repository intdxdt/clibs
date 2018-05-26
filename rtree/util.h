#include <cmath>
#include <limits>
#include "../mbr/mbr.h"

#ifndef RTREE_CPP_UTIL_H
#define RTREE_CPP_UTIL_H

std::array<double, 4> emptyBounds() {
    auto inf = std::numeric_limits<double>::infinity();
    auto neginf = -std::numeric_limits<double>::infinity();
    return {inf, inf, neginf, neginf};
}

MBR emptyMbr() {
    return {emptyBounds(), true};
}



#endif //RTREE_CPP_UTIL_H
