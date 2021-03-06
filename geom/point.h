//
//06/06/18.
//
#include "util.h"
#include "geometry.h"
#include "../mutil/mutil.h"

#ifndef GEOM_POINT_H
#define GEOM_POINT_H

namespace geom {
    struct Point : Geometry {
        double x;
        double y;
        double z;

        Point(double x, double y) : x(x), y(y), z(std::nan("0")) {}

        Point(double x, double y, double z) : x(x), y(y), z(z) {}

        explicit Point(const std::vector<double>& v) :
                x(v[0]), y(v[1]), z(v.size() > 2 ? v[2] : std::nan("0")) {}

        ///clone
        Point clone() {
            return {x, y, z};
        }

        ///is null
        bool is_null() {
            return std::isnan(x) || std::isnan(y);
        }

        ///Is point zero in 2d - origin
        bool is_zero() {
            return feq(x, 0.0) && feq(y, 0.0);
        }

        ///Area of point
        double area() {
            return 0.0;
        }

        ///checks if a point is a ring , by def every point is a ring
        /// which concides on itself
        bool is_ring() {
            return true;
        }


        ///compare equality in 2d
        bool equals_2d(const Point& other) const {
            return feq(x, other.x) && feq(y, other.y);
        }
    };
}

#endif //GEOM_POINT_H
