//
//06/06/18.
//
#include <cmath>

#ifndef GEOM_UTIL_H
#define GEOM_UTIL_H
namespace geom {
    double inf = std::numeric_limits<double>::infinity();
    double neginf = -std::numeric_limits<double>::infinity();

    //find the sign of value -1, 0 , 1
    inline int xy_sign(double v) const {
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
