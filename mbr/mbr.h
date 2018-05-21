#include <cmath>
#include <array>
#include <vector>
#include <sstream>
#include <cassert>
#include <functional>
#include <optional>

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

        double operator[](const int index) {
            assert(index >= 0 && index < 4);
            return index == 0 ? minx :
                   index == 1 ? miny :
                   index == 2 ? maxx :
                   index == 3 ? maxy : std::nan("-9");
        }

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

        std::array<f64, 4> as_array() {
            return {minx, miny, maxx, maxy};
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

        ///Computes the center of minimum bounding box - (x, y)
        std::vector<f64> center() const {
            return {(minx + maxx) / 2.0, (miny + maxy) / 2.0};
        }

        ///Checks if bounding box intersects other
        bool intersects(const MBR& other) const {
            //not disjoint
            return !(other.minx > maxx ||
                     other.maxx < minx ||
                     other.miny > maxy ||
                     other.maxy < miny);
        }

        ///intersects point
        bool intersects(f64 x, f64 y) const {
            return contains(x, y);
        }

        ///Test for disjoint between two mbrs
        bool disjoint(const MBR& other) const {
            return !intersects(other);
        }

        ///Computes the intersection of two bounding box
        std::optional<MBR> intersection(const MBR& other) {
            if (disjoint(other)) {
                return std::nullopt;
            }
            f64 minx_ = minx > other.minx ? minx : other.minx;
            f64 miny_ = miny > other.miny ? miny : other.miny;
            f64 maxx_ = maxx < other.maxx ? maxx : other.maxx;
            f64 maxy_ = maxy < other.maxy ? maxy : other.maxy;
            return MBR{minx_, miny_, maxx_, maxy_};
        }


        ///Expand include other bounding box
        MBR& expand_to_include(const MBR& other) {
            if (other.minx < minx) {
                minx = other.minx;
            }

            if (other.maxx > maxx) {
                maxx = other.maxx;
            }

            if (other.miny < miny) {
                miny = other.miny;
            }

            if (other.maxy > maxy) {
                maxy = other.maxy;
            }

            return *this;
        }


        ///Expand to include x,y
        MBR& expand_to_include(f64 x, f64 y) {
            if (x < minx) {
                minx = x;
            }
            else if (x > maxx) {
                maxx = x;
            }

            if (y < miny) {
                miny = y;
            }
            else if (y > maxy) {
                maxy = y;
            }
            return *this;
        }

        ///Expand by delta in x and y
        MBR& expand_by_delta(f64 dx, f64 dy) {
            f64 minx_ = minx - dx, miny_ = miny - dy;
            f64 maxx_ = maxx + dx, maxy_ = maxy + dy;

            minx = fmin(minx_, maxx_);
            miny = fmin(miny_, maxy_);
            maxx = fmax(minx_, maxx_);
            maxy = fmax(miny_, maxy_);

            return *this;
        }


        ///computes dx and dy for computing hypot
        xy distance_dxdy(const MBR& other) {
            f64 dx = 0.0;
            f64 dy = 0.0;

            // find closest edge by x
            if (maxx < other.minx) {
                dx = other.minx - maxx;
            }
            else if (minx > other.maxx) {
                dx = minx - other.maxx;
            }

            // find closest edge by y
            if (maxy < other.miny) {
                dy = other.miny - maxy;
            }
            else if (miny > other.maxy) {
                dy = miny - other.maxy;
            }

            return xy{dx, dy};
        }


        ///Distance computes the distance between two mbrs
        f64 distance(const MBR& other) {
            if (intersects(other)) {
                return 0.0;
            }
            auto dxdy = distance_dxdy(other);
            return std::hypot(std::get<0>(dxdy), std::get<1>(dxdy));
        }

        ///distance square computes the squared distance
        ///between bounding boxes
        f64 distance_square(const MBR& other) {
            if (intersects(other)) {
                return 0.0;
            }
            auto dxdy = distance_dxdy(other);
            f64 dx = std::get<0>(dxdy), dy = std::get<1>(dxdy);
            return (dx * dx) + (dy * dy);
        }

        std::string wkt() const {
            std::ostringstream ss;
            ss << "POLYGON (("
               << minx << " " << miny << ","
               << minx << " " << maxy << ","
               << maxx << " " << maxy << ","
               << maxx << " " << miny << ","
               << minx << " " << miny
               << "))";
            return std::move(ss.str());
        }

        ///operator : +
        MBR& operator+(const MBR& other) { return expand_to_include(other); }

        ///operator : | or +
        MBR& operator|(const MBR& other) { return *this + other; }

        ///operator : & : intersection
        std::optional<MBR> operator&(const MBR& other) { return intersection(other); }

        ///operator : equals
        bool operator==(const MBR& other) { return equals(other); }

        ///operator : not equal
        bool operator!=(const MBR& other) { return !(*this == other); }

    };

}

#endif //MBR_MBR_H
