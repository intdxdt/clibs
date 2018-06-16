//
//16/06/18.
//
#include "point.h"

#ifndef GEOM_COORDS_H
#define GEOM_COORDS_H
namespace geom {
    //Is coordinates a ring : P0 == Pn
    bool is_ring(std::vector <Point> coordinates) {
        if (coordinates.size() < 2) {
            return false;
        }
        return coordinates[0].equals_2d(coordinates[coordinates.size() - 1]);
    }
}
#endif //GEOM_COORDS_H
