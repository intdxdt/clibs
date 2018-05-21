#include <cmath>
#include <vector>

#ifndef MBR_MBR_H
#define MBR_MBR_H
namespace mbr {
    using f64 = double;
    const f64 EPSILON = 1.0e-12;
    using xy = std::tuple<f64, f64>;


    bool feq(f64 a, f64 b) {
        return a == b || fabs(a - b) < EPSILON;
    }

    struct MBR {
        f64 minx;
        f64 miny;
        f64 maxx;
        f64 maxy;

        MBR(f64 minx, f64 miny, f64 maxx, f64 maxy) :
                minx(fmin(minx, maxx)), miny(fmin(miny, maxy)),
                maxx(fmax(minx, maxx)), maxy(fmax(miny, maxy)) {}

        MBR(f64 minx, f64 miny, f64 maxx, f64 maxy, bool raw) :
                minx(minx), miny(miny), maxx(maxx), maxy(maxy) {}

        f64 width() const { return maxx - minx; }

        f64 height() const { return maxy - miny; }

        f64 area() const { return height() * width(); }

        std::vector<std::vector<f64>> as_poly_array() {
            return {
                    {minx, miny},
                    {minx, maxy},
                    {maxx, maxy},
                    {maxx, miny},
                    {minx, miny},
            };
        }

        std::tuple<f64, f64, f64, f64> as_tuple() {
            return std::tuple<f64, f64, f64, f64>{
                    minx, miny, maxx, maxy
            };
        }

        std::tuple<xy, xy> llur() {
            return std::tuple<xy, xy>{
                    xy{minx, miny}, xy{maxx, maxy}
            };
        }

        ///Compare equality of two minimum bounding box
        bool equals(const MBR& other) const {
            return feq(maxx, other.maxx) &&
                   feq(maxy, other.maxy) &&
                   feq(minx, other.minx) &&
                   feq(miny, other.miny);
        }

        ///Checks if bounding box can be represented as a point,
        /// has both width and height as 0.
        bool is_point() const {
            return feq(height(), 0) && feq(width(), 0);
        }

        ///Contains bonding box
        ///is true if mbr completely contains other, boundaries may touch
        bool contains(const MBR& other) const {
            return (other.minx >= minx) &&
                   (other.miny >= miny) &&
                   (other.maxx <= maxx) &&
                   (other.maxy <= maxy);
        }

        ///contains x, y
        bool contains(f64 x, f64 y) const {
            return (x >= minx) &&
                   (x <= maxx) &&
                   (y >= miny) &&
                   (y <= maxy);
        }

        ///Completely contains bonding box
        ///is true if mbr completely contains other
        /// without touching boundary
        bool completely_contains(const MBR& other) const {
            return (other.minx > minx) &&
                   (other.miny > miny) &&
                   (other.maxx < maxx) &&
                   (other.maxy < maxy);
        }

        ///completely_contains_xy is true if mbr completely
        /// contains location with {x, y} without touching boundary
        bool completely_contains(f64 x, f64 y) const {
            return (x > minx) &&
                   (x < maxx) &&
                   (y > miny) &&
                   (y < maxy);
        }

        ///Create new bounding box by translating by dx and dy.
        MBR translate(f64 dx, f64 dy) const {
            return {
                    minx + dx, miny + dy,
                    maxx + dx, maxy + dy
            };
        }
    };

}

#endif //MBR_MBR_H
