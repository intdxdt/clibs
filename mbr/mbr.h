#include <cmath>
#include <cassert>
#include <array>
#include <vector>
#include <sstream>
#include <optional>
#include <iomanip>
#include <functional>

#include "../pt/pt.h"
#include "../mutil/mutil.h"

#ifndef MBR_MBR_H
#define MBR_MBR_H
namespace mbr {
    template<typename T>
    struct MBR {
        T minx;
        T miny;
        T maxx;
        T maxy;

        MBR(T minx, T miny, T maxx, T maxy) :
                minx(fmin(minx, maxx)), miny(fmin(miny, maxy)),
                maxx(fmax(minx, maxx)), maxy(fmax(miny, maxy)) {}

        MBR(T minx_, T miny_, T maxx_, T maxy_, bool raw) :
                minx(raw ? minx_ : fmin(minx_, maxx_)),
                miny(raw ? miny_ : fmin(miny_, maxy_)),
                maxx(raw ? maxx_ : fmax(minx_, maxx_)),
                maxy(raw ? maxy_ : fmax(miny_, maxy_)) {
        }

        explicit MBR(const std::array<T, 4> &bounds) :
                minx(fmin(bounds[0], bounds[2])),
                miny(fmin(bounds[1], bounds[3])),
                maxx(fmax(bounds[0], bounds[2])),
                maxy(fmax(bounds[1], bounds[3])) {
        }

        MBR(const std::array<T, 4> &bounds, bool raw) {
            *this = MBR(bounds[0], bounds[1], bounds[2], bounds[3], raw);
        }

        bool operator<(const MBR &other) {
            auto d = minx - other.minx;
            if (feq(d, 0)) {
                d = miny - other.miny;
            }
            return d < 0;
        }

        MBR &bbox() { return *this; }

        MBR clone() { return *this; }

        T width() const { return maxx - minx; }

        T height() const { return maxy - miny; }

        T area() const { return height() * width(); }

        std::vector<std::vector<T>> as_poly_array() {
            return {
                    {minx, miny},
                    {minx, maxy},
                    {maxx, maxy},
                    {maxx, miny},
                    {minx, miny},
            };
        }

        std::array<T, 4> as_array() {
            return std::array<T, 4>{minx, miny, maxx, maxy};
        }

        std::tuple<T, T, T, T> as_tuple() {
            return std::tuple<T, T, T, T>{
                    minx, miny, maxx, maxy
            };
        }

        std::tuple<Pt, Pt> llur() {
            return std::tuple<Pt, Pt>{{minx, miny},
                                      {maxx, maxy}};
        }

        ///Compare equality of two minimum bounding box
        bool equals(const MBR &other) const {
            return eqls(maxx, other.maxx) &&
                   eqls(maxy, other.maxy) &&
                   eqls(minx, other.minx) &&
                   eqls(miny, other.miny);
        }

        ///Checks if bounding box can be represented as a point,
        /// has both width and height as 0.
        bool is_point() const {
            return feq(height(), 0) && feq(width(), 0);
        }

        ///Contains bonding box
        ///is true if mbr completely contains other, boundaries may touch
        bool contains(const MBR &other) const {
            return (other.minx >= minx) &&
                   (other.miny >= miny) &&
                   (other.maxx <= maxx) &&
                   (other.maxy <= maxy);
        }

        ///contains x, y
        bool contains(T x, T y) const {
            return (x >= minx) &&
                   (x <= maxx) &&
                   (y >= miny) &&
                   (y <= maxy);
        }

        ///Completely contains bonding box
        ///is true if mbr completely contains other
        /// without touching boundary
        bool completely_contains(const MBR &other) const {
            return (other.minx > minx) &&
                   (other.miny > miny) &&
                   (other.maxx < maxx) &&
                   (other.maxy < maxy);
        }

        ///completely_contains_xy is true if mbr completely
        /// contains location with {x, y} without touching boundary
        bool completely_contains(T x, T y) const {
            return (x > minx) &&
                   (x < maxx) &&
                   (y > miny) &&
                   (y < maxy);
        }

        ///Create new bounding box by translating by dx and dy.
        MBR translate(T dx, T dy) const {
            return {minx + dx, miny + dy, maxx + dx, maxy + dy};
        }

        ///Computes the center of minimum bounding box - (x, y)
        Pt center() const {
            return {(minx + maxx) / 2.0, (miny + maxy) / 2.0};
        }

        ///Checks if bounding box intersects other
        bool intersects(const MBR &other) const {
            //not disjoint
            return !(other.minx > maxx ||
                     other.maxx < minx ||
                     other.miny > maxy ||
                     other.maxy < miny);
        }

        ///intersects point
        bool intersects(T x, T y) const {
            return contains(x, y);
        }

        ///intersects point
        bool intersects(const Pt &pt1, const Pt &pt2) const {
            auto minq = min(pt1.x, pt2.x);
            auto maxq = max(pt1.x, pt2.x);

            if (minx > maxq || maxx < minq) {
                return false;
            }

            minq = min(pt1.y, pt2.y);
            maxq = max(pt1.y, pt2.y);

            // not disjoint
            return !(miny > maxq || maxy < minq);
        }

        ///Test for disjoint between two mbrs
        bool disjoint(const MBR &other) const {
            return !intersects(other);
        }

        ///Computes the intersection of two bounding box
        std::optional<MBR> intersection(const MBR &other) {
            if (disjoint(other)) {
                return std::nullopt;
            }
            auto minx_ = minx > other.minx ? minx : other.minx;
            auto miny_ = miny > other.miny ? miny : other.miny;
            auto maxx_ = maxx < other.maxx ? maxx : other.maxx;
            auto maxy_ = maxy < other.maxy ? maxy : other.maxy;
            return MBR{minx_, miny_, maxx_, maxy_};
        }


        ///Expand include other bounding box
        MBR &expand_to_include(const MBR &other) {
            minx = min(other.minx, minx);
            miny = min(other.miny, miny);

            maxx = max(other.maxx, maxx);
            maxy = max(other.maxy, maxy);
            return *this;
        }


        ///Expand to include x,y
        MBR &expand_to_include(double x, T y) {
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
        MBR &expand_by_delta(T dx, T dy) {
            auto minx_ = minx - dx, miny_ = miny - dy;
            auto maxx_ = maxx + dx, maxy_ = maxy + dy;

            minx = min(minx_, maxx_);
            miny = min(miny_, maxy_);
            maxx = max(minx_, maxx_);
            maxy = max(miny_, maxy_);

            return *this;
        }


        ///computes dx and dy for computing hypot
        Pt distance_dxdy(const MBR &other) const {
            T dx{};
            T dy{};

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

            return Pt{dx, dy};
        }


        ///Distance computes the distance between two mbrs
        double distance(const MBR &other) const {
            if (intersects(other)) {
                return 0.0;
            }
            auto o = distance_dxdy(other);
            return std::hypot(o.x, o.y);
        }

        ///distance square computes the squared distance
        ///between bounding boxes
        double distance_square(const MBR &other) {
            if (intersects(other)) {
                return 0.0;
            }
            auto o = distance_dxdy(other);
            return (o.x * o.x) + (o.y * o.y);
        }

        ///WKT : wkt string of mbr as polygon
        std::string wkt() const {
            std::ostringstream ss;
            ss << "POLYGON (("
               << f2str(minx) << " " << f2str(miny) << ", "
               << f2str(minx) << " " << f2str(maxy) << ", "
               << f2str(maxx) << " " << f2str(maxy) << ", "
               << f2str(maxx) << " " << f2str(miny) << ", "
               << f2str(minx) << " " << f2str(miny)
               << "))";
            return ss.str();
        }

        ///Operator : +
        MBR operator+(const MBR &other) {
            return {
                    min(other.minx, minx),
                    min(other.miny, miny),
                    max(other.maxx, maxx),
                    max(other.maxy, maxy),
                    true
            };
        }

        ///Operator : | or +
        MBR operator|(const MBR &other) {
            return *this + other;
        }

        ///Operator : & : intersection
        std::optional<MBR> operator&(const MBR &other) {
            return intersection(other);
        }

        ///Operator : equals
        bool operator==(const MBR &other) {
            return equals(other);
        }

        ///Operator : not equal
        bool operator!=(const MBR &other) {
            return !(*this == other);
        }

    private:
        std::string f2str(double f, int precision = 12) const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(precision) << f;
            auto s = ss.str();
            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
            if (s.back() == '.') {
                s.pop_back();
            }
            return s;
        }

        template<typename U>
        auto min(U a, U b) const {
            if constexpr (std::is_integral<T>::value) {
                return b < a ? b : a;
            }
            else {
                return fmin(a, b);
            };
        }

        template<typename U>
        auto max(U a, U b) const {
            if constexpr (std::is_integral<T>::value) {
                return b > a ? b : a;
            }
            else {
                return fmax(a, b);
            };
        }


        template<typename U>
        auto eqls(U a, U b) const {
            if constexpr (std::is_integral<T>::value) {
                return a == b;
            }
            else {
                return feq(a, b);
            };
        }

    };
}

#endif //MBR_MBR_H
