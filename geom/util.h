//
//06/06/18.
//
#include <cmath>
#include <vector>
#include <cassert>
#include <array>
#include <vector>
#include <sstream>
#include <functional>
#include <optional>
#include <iomanip>
#include "../mbr/mbr.h"
#include "../mutil/mutil.h"
#include "../rtree/rtree.h"

#ifndef GEOM_UTIL_H
#define GEOM_UTIL_H

namespace geom {
    double inf = std::numeric_limits<double>::infinity();
    double neginf = -std::numeric_limits<double>::infinity();
    const int x = 0;
    const int y = 1;
    const int z = 2;
    const int null = -9;

    enum GeoType {
        UnkownType = -1,
        PointType = 0,
        SegmentType = 1,
        LineStringType = 2,
        PolygonType = 3,
    };

    //find the sign of value -1, 0 , 1
    inline int xy_sign(double v)  {
        int i{0};
        if (v > 0) {
            i = 1;
        }
        else if (v < 0) {
            i = -1;
        }
        return i;
    }
}

#endif //GEOM_UTIL_H
