//
//15/06/18.
//
#include "coords.h"
#include "line.h"

#ifndef GEOM_RING_H
#define GEOM_RING_H
namespace geom {
    struct LinearRing {
        std::shared_ptr<LineString> linestr{nullptr};
        explicit LinearRing(std::vector<Point> coordinates) {
            auto n = coordinates.size();
            if (n > 1) {
                if (!is_ring(coordinates)) {
                    coordinates.emplace_back(coordinates[0]);
                }
            }
            linestr = std::make_shared<LineString>(LineString(coordinates));
        }
    };
}
#endif //GEOM_RING_H
