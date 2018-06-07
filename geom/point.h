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
#include "../mutil/mutil.h"

#ifndef GEOM_POINT_H
#define GEOM_POINT_H
namespace geom {

    struct Point {
        double x;
        double y;
        double z;

        Point(double x, double y) : x(x), y(y), z(std::nan("0")) {}

        Point(double x, double y, double z) : x(x), y(y), z(z) {}

        explicit Point(const std::vector<double>& v) :
                x(v[0]), y(v[1]), z(v.size() > 2 ? v[2] : std::nan("0")) {}

        ///is null
        bool is_null() {
            return std::isnan(x) || std::isnan(y);
        }

        ///indexing
        double operator[](std::size_t index) const {
            assert(index >= 0 && index < 2);
            return index == 0 ? x :
                   index == 1 ? y :
                   index == 2 ? z : std::nan("-9");
        }

        ///compare equality in 2d
        bool equals_2d(const Point& other) const {
            return feq(x, other.x) && feq(y, other.y);
        }
    };
}
#endif //GEOM_POINT_H
