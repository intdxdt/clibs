#include <cmath>
#include <cassert>
#include "../mutil/mutil.h"

#ifndef PT_PT_H
#define PT_PT_H

struct Pt2D {
    double x;
    double y;

    double operator[](const int index) {
        assert(index >= 0 && index < 2);
        return index == 0 ? x :
               index == 1 ? y : std::nan("-9");
    }

    ///Operator : equals
    bool operator==(const Pt2D& other) {
        return feq(x, other.x) && feq(y, other.y);
    }

    ///Operator : not equal
    bool operator!=(const Pt2D& other) {
        return !(*this == other);
    }
};

struct Pt3D {
    double x;
    double y;
    double z;

    double operator[](const int index) {
        assert(index >= 0 && index < 3);
        return index == 0 ? x :
               index == 1 ? y :
               index == 2 ? z : std::nan("-9");
    }

    ///Operator : equals
    bool operator==(const Pt3D& other) {
        return feq(x, other.x) && feq(y, other.y) && feq(z, other.z);
    }

    ///Operator : not equal
    bool operator!=(const Pt3D& other) {
        return !(*this == other);
    }
};

#endif //PT_PT_H
