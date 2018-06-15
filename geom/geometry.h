//
//06/06/18.
//
#include "util.h"

#ifndef GEOM_GEOMETRY_H
#define GEOM_GEOMETRY_H

namespace geom {
    struct Geometry {
        virtual MBR BBox() {
            return MBR{std::nan("0"), std::nan("0"), std::nan("0"), std::nan("0"), true};
        }

        virtual std::vector<Pt2D> AsLinear() {
            return {};
        }

        virtual bool Intersects(Geometry) {
            return false;
        }

        virtual std::vector<Pt2D> Intersection(Geometry) {
            return {};
        }

        virtual double Distance(Geometry) {
            return std::nan("0");
        }

        virtual GeoType Type() {
            return GeoType::UnkownType;
        }

        virtual std::string WKT() {
            return "Unknown";
        }
    };
}

#endif //GEOM_GEOMETRY_H
