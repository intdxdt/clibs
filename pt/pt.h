#include <cmath>
#include <cassert>
#include <array>
#include "../mutil/mutil.h"

#ifndef PT_PT_H
#define PT_PT_H

struct Pt2D {
    double x;
    double y;

    ///Operator : equals
    inline bool equals(const Pt2D& other) {
        return feq(x, other.x) && feq(y, other.y);
    }

    ///Operator : equals
    inline bool operator==(const Pt2D& other) {
        return equals(other);
    }

    ///Operator : not equal
    inline bool operator!=(const Pt2D& other) {
        return !equals(other);
    }

    ///As Array
    std::array<double, 2> as_array() {
        return {x, y};
    }
};

struct Pt3D {
    double x;
    double y;
    double z;

    ///Operator : equals
    inline bool equals(const Pt3D& other) {
        return feq(x, other.x) && feq(y, other.y) && feq(z, other.z);
    }

    ///Operator : equals
    inline bool operator==(const Pt3D& other) {
        return equals(other);
    }

    ///Operator : not equal
    inline bool operator!=(const Pt3D& other) {
        return !equals(other);
    }

    ///As Array
    std::array<double, 3> as_array() {
        return {x, y, z};
    }
};

#endif //PT_PT_H
