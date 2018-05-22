#include <cmath>
#include <array>
#include <vector>
#include <sstream>
#include <cassert>
#include <functional>
#include <optional>
#include "../pt/pt.h"
#include "../mutil/mutil.h"

#ifndef MBR_MBR_H
#define MBR_MBR_H
namespace mbr {
    struct MBR {
        double minx;
        double miny;
        double maxx;
        double maxy;

        MBR(double minx, double miny, double maxx, double maxy) :
                minx(fmin(minx, maxx)), miny(fmin(miny, maxy)),
                maxx(fmax(minx, maxx)), maxy(fmax(miny, maxy)) {}

        MBR(double minx_, double miny_, double maxx_, double maxy_, bool raw) {
            minx = raw ? minx_ : fmin(minx_, maxx_);
            maxx = raw ? maxx_ : fmax(minx_, maxx_);
            miny = raw ? miny_ : fmin(miny_, maxy_);
            maxy = raw ? maxy_ : fmax(miny_, maxy_);
        }

        double operator[](const int index) {
            assert(index >= 0 && index < 4);
            return index == 0 ? minx :
                   index == 1 ? miny :
                   index == 2 ? maxx :
                   index == 3 ? maxy : std::nan("-9");
        }

        double width() const { return maxx - minx; }

        double height() const { return maxy - miny; }

        double area() const { return height() * width(); }

        std::vector<std::vector<double>> as_poly_array() {
            return {
                    {minx, miny},
                    {minx, maxy},
                    {maxx, maxy},
                    {maxx, miny},
                    {minx, miny},
            };
        }

        std::array<double, 4> as_array() {
            return {minx, miny, maxx, maxy};
        }

        std::tuple<double, double, double, double> as_tuple() {
            return std::tuple<double, double, double, double>{
                    minx, miny, maxx, maxy
            };
        }

        std::tuple<Pt2D, Pt2D> llur() {
            return std::tuple<Pt2D, Pt2D>{
                    Pt2D{minx, miny}, Pt2D{maxx, maxy}
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
        bool contains(double x, double y) const {
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
        bool completely_contains(double x, double y) const {
            return (x > minx) &&
                   (x < maxx) &&
                   (y > miny) &&
                   (y < maxy);
        }

        ///Create new bounding box by translating by dx and dy.
        MBR translate(double dx, double dy) const {
            return {minx + dx, miny + dy, maxx + dx, maxy + dy};
        }

        ///Computes the center of minimum bounding box - (x, y)
        std::vector<double> center() const {
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
        bool intersects(double x, double y) const {
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
            double minx_ = minx > other.minx ? minx : other.minx;
            double miny_ = miny > other.miny ? miny : other.miny;
            double maxx_ = maxx < other.maxx ? maxx : other.maxx;
            double maxy_ = maxy < other.maxy ? maxy : other.maxy;
            return MBR{minx_, miny_, maxx_, maxy_};
        }


        ///Expand include other bounding box
        MBR& expand_to_include(const MBR& other) {
            minx = fmin(other.minx, minx);
            miny = fmin(other.miny, miny);

            maxx = fmax(other.maxx, maxx);
            maxy = fmax(other.maxy, maxy);
            return *this;
        }


        ///Expand to include x,y
        MBR& expand_to_include(double x, double y) {
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
        MBR& expand_by_delta(double dx, double dy) {
            double minx_ = minx - dx, miny_ = miny - dy;
            double maxx_ = maxx + dx, maxy_ = maxy + dy;

            minx = fmin(minx_, maxx_);
            miny = fmin(miny_, maxy_);
            maxx = fmax(minx_, maxx_);
            maxy = fmax(miny_, maxy_);

            return *this;
        }


        ///computes dx and dy for computing hypot
        Pt2D distance_dxdy(const MBR& other) {
            double dx = 0.0;
            double dy = 0.0;

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

            return Pt2D{dx, dy};
        }


        ///Distance computes the distance between two mbrs
        double distance(const MBR& other) {
            if (intersects(other)) {
                return 0.0;
            }
            auto pt = distance_dxdy(other);
            return std::hypot(pt.x, pt.y);
        }

        ///distance square computes the squared distance
        ///between bounding boxes
        double distance_square(const MBR& other) {
            if (intersects(other)) {
                return 0.0;
            }
            auto pt = distance_dxdy(other);
            return (pt.x * pt.x) + (pt.y * pt.y);
        }

        ///WKT : wkt string of mbr as polygon
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

        ///Operator : +
        MBR operator+(const MBR& other) {
            return {
                    fmin(other.minx, minx),
                    fmin(other.miny, miny),
                    fmax(other.maxx, maxx),
                    fmax(other.maxy, maxy),
                    true
            };
        }

        ///Operator : | or +
        MBR operator|(const MBR& other) {
            return *this + other;
        }

        ///Operator : & : intersection
        std::optional<MBR> operator&(const MBR& other) {
            return intersection(other);
        }

        ///Operator : equals
        bool operator==(const MBR& other) {
            return equals(other);
        }

        ///Operator : not equal
        bool operator!=(const MBR& other) {
            return !(*this == other);
        }

    };

}

#endif //MBR_MBR_H
