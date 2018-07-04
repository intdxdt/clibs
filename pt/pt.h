#include <cmath>
#include <cassert>
#include <array>
#include "../mutil/mutil.h"

#ifndef PT_PT_H
#define PT_PT_H

struct Pt2D {
    double x;
    double y;

    double operator[](const size_t index) {
        assert(index >= 0 && index < 2);
        return index == 0 ? x :
               index == 1 ? y : std::nan("-9");
    }

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

    inline double operator[](const size_t index) {
        assert(index >= 0 && index < 3);
        return index == 0 ? x :
               index == 1 ? y :
               index == 2 ? z : std::nan("-9");
    }

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
