//
//06/06/18.
//
#include "../pt/pt.h"
#include "../mbr/mbr.h"

#ifndef GEOM_GEOM_H
#define GEOM_GEOM_H
namespace geom {
    const int x = 0;
    const int y = 1;
    const int z = 2;
    const int null = -9;

    enum GeoType {
        Unkown = -1,
        Point = 0,
        Segment = 1,
        LineString = 2,
        Polygon = 3,
    };

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
            return GeoType::Unkown;
        }

        virtual std::string WKT() {
            return "Unknown";
        }
    };
}
#endif //GEOM_GEOM_H
