//
//16/06/18.
//
#include "point.h"

#ifndef GEOM_COORDS_H
#define GEOM_COORDS_H
namespace geom {

    //is coordinates a ring : P0 == Pn
    bool is_ring(std::vector<Point> coordinates) {
        if (coordinates.size() < 2) {
            return false;
        }
        return coordinates[0].equals_2d(coordinates[coordinates.size() - 1]);
    }

    //coordinates as 2d array
    std::vector<std::array<double, 2>> coordinates_as_array(std::vector<Point> coordinates) {
        auto n = coordinates.size();
        std::vector<std::array<double, 2>> coords;
        coords.reserve(n);
        for (size_t i = 0; i < n; i++) {
            coords.emplace_back(std::array<double, 2>{coordinates[i][x], coordinates[i][y]});
        }
        return coords;
    }
}
#endif //GEOM_COORDS_H
