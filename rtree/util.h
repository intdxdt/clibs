#include <limits>
#include <memory>
#include <utility>
#include <tuple>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <functional>
#include "../mbr/mbr.h"

#ifndef RTREE_CPP_UTIL_H
#define RTREE_CPP_UTIL_H
namespace rtree {
    using SortBy = std::size_t;
    constexpr SortBy ByX = 0;
    constexpr SortBy ByY = 1;
    constexpr double Inf = std::numeric_limits<double>::infinity();
    constexpr double NegInf = -std::numeric_limits<double>::infinity();

    inline std::array<double, 4> empty_bounds() {
        return {Inf, Inf, NegInf, NegInf};
    }

    //@formatter:off
    inline mbr::MBR empty_mbr() {
        return {empty_bounds(), true};
    }

    template<typename T>
    inline size_t len(const std::vector<T>& v) {
        return v.size();
    };

    template<typename T>
    inline const T& min(const T& a, const T& b) {
        return b < a ? b : a;
    };

    template<typename T>
    inline const T& max(const T& a, const T& b) {
        return b > a ? b : a;
    };

    template<typename T>
    T pop(std::vector<T>& a) {
        if (a.empty()) {
            return nullptr;
        }
        auto v = a.back();
        a.resize(a.size() - 1);
        return std::move(v);
    }

    ///std::optional<size_t>
    template <>
    size_t pop(std::vector<size_t>& a) {
        auto v = a.back();
        a.resize(a.size() - 1);
        return v;
    }

    template<typename T>
    std::vector<T> slice(const std::vector<T>& v, size_t i = 0, size_t j = 0) {
        std::vector<T> s(v.begin() + i, v.begin() + j);
        return std::move(s);
    }

    template <typename T>
    inline void swap_item(std::vector<T*>& arr, size_t i, size_t j) {
        std::swap(arr[i], arr[j]);
    }


    ///slice index
    inline std::optional<size_t> slice_index(size_t limit, const std::function<bool(size_t)>& predicate) {
        bool bln{false};
        size_t index{0};

        for (size_t i = 0; !bln && i < limit; ++i) {
            index = i;
            bln = predicate(i);
        }
        return bln ? std::optional<size_t>{index} : std::nullopt;
    }


    ///extend bounding box
    inline mbr::MBR& extend(mbr::MBR& a, const mbr::MBR& b) {
        a.minx = std::fmin(a.minx, b.minx);
        a.miny = std::fmin(a.miny, b.miny);
        a.maxx = std::fmax(a.maxx, b.maxx);
        a.maxy = std::fmax(a.maxy, b.maxy);
        return a;
    }

    ///computes area of bounding box
    inline double bbox_area(const mbr::MBR& a) {
        return (a.maxx - a.minx) * (a.maxy - a.miny);
    }

    ///computes box margin
    inline double bbox_margin(const mbr::MBR& a) {
        return (a.maxx - a.minx) + (a.maxy - a.miny);
    }

    ///computes enlarged area given two mbrs
    inline double enlarged_area(const mbr::MBR& a, const mbr::MBR& b) {
        return (std::fmax(a.maxx, b.maxx) - std::fmin(a.minx, b.minx)) *
               (std::fmax(a.maxy, b.maxy) - std::fmin(a.miny, b.miny));
    }

    ///contains tests whether a contains b
    inline bool contains(const mbr::MBR& a, const mbr::MBR& b) {
        return a.contains(b);
    }

    ///intersects tests a intersect b (mbr)
    inline bool intersects(const mbr::MBR& a, const mbr::MBR& b) {
        return a.intersects(b);
    }

    ///computes the intersection area of two mbrs
    inline double intersection_area(const mbr::MBR& a, const mbr::MBR& b) {
        if (a.disjoint(b)) {
            return 0.0;
        }
        auto minx = (b.minx > a.minx) ? b.minx : a.minx;
        auto miny = (b.miny > a.miny) ? b.miny : a.miny;
        auto maxx = (b.maxx < a.maxx) ? b.maxx : a.maxx;
        auto maxy = (b.maxy < a.maxy) ? b.maxy : a.maxy;
        return (maxx - minx) * (maxy - miny);
    }
}
#endif //RTREE_CPP_UTIL_H
