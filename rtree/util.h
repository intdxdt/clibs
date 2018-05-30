#include <cmath>
#include <limits>
#include <algorithm>
#include "../mbr/mbr.h"

#ifndef RTREE_CPP_UTIL_PROTOS
#define RTREE_CPP_UTIL_PROTOS

//@formatter:off
MBR empty_mbr();
struct Object;
double bbox_area(const MBR&);
MBR& extend(MBR&, const MBR&);
double bbox_margin(const MBR&);
std::array<double, 4> empty_bounds();
bool contains(const MBR& , const MBR&) ;
bool intersects(const MBR&, const MBR&);
double enlarged_area(const MBR&, const MBR&);
double intersection_area(const MBR&, const MBR&);
std::optional<size_t> slice_index(size_t, const std::function<bool(size_t)>&);
void swap_item(std::vector<Object>&, std::size_t, std::size_t);
template<typename T>
std::vector<T> split_at_index(std::vector<T>& v, std::size_t index);
//@formatter:on
#endif

#ifndef RTREE_CPP_OBJECT_H
#define RTREE_CPP_OBJECT_H

struct Object {
    size_t id = 0;
    MBR bbox = empty_mbr();
    void* object = nullptr;
    size_t meta = 0;
};
#endif //RTREE_CPP_OBJECT_H

#ifndef RTREE_CPP_UTIL_H
#define RTREE_CPP_UTIL_H
using SortBy = std::size_t;
const SortBy ByX = 0;
const SortBy ByY = 1;

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
size_t pop_index(std::vector<size_t>& indexes) {
    assert(!indexes.empty());
    size_t v = indexes.back();
    indexes.resize(indexes.size() - 1);
    return v;
}

template<typename T>
std::vector<T> slice(const std::vector<T>& v, size_t i = 0, size_t j = 0) {
    std::vector<T> s(v.begin() + i, v.begin() + j);
    return std::move(s);
}

//split at index
template<typename T>
std::vector<T> split_at_index(std::vector<T>& v, std::size_t index) {
    std::vector<T> part(v.begin() + index, v.end());
    v.resize(index);
    return std::move(part);
}

void swap_item(std::vector<Object>& arr, size_t i, size_t j) {
    std::swap(arr[i], arr[j]);
}

std::array<double, 4> empty_bounds() {
    auto inf = std::numeric_limits<double>::infinity();
    auto neginf = -std::numeric_limits<double>::infinity();
    return {inf, inf, neginf, neginf};
}

MBR empty_mbr() {
    return {empty_bounds(), true};
}

//slice index
std::optional<size_t> slice_index(size_t limit, const std::function<bool(size_t)>& predicate) {
    for (size_t i = 0; i < limit; ++i) {
        if (predicate(i))
            return i;
    }
    return std::nullopt;
}


//extend bounding box
MBR& extend(MBR& a, const MBR& b) {
    a.minx = std::fmin(a.minx, b.minx);
    a.miny = std::fmin(a.miny, b.miny);
    a.maxx = std::fmax(a.maxx, b.maxx);
    a.maxy = std::fmax(a.maxy, b.maxy);
    return a;
}

//computes area of bounding box
double bbox_area(const MBR& a) {
    return (a.maxx - a.minx) * (a.maxy - a.miny);
}

//computes box margin
double bbox_margin(const MBR& a) {
    return (a.maxx - a.minx) + (a.maxy - a.miny);
}


//computes enlarged area given two mbrs
double enlarged_area(const MBR& a, const MBR& b) {
    return (std::fmax(a.maxx, b.maxx) - std::fmin(a.minx, b.minx)) *
           (std::fmax(a.maxy, b.maxy) - std::fmin(a.miny, b.miny));
}

//contains tests whether a contains b
bool contains(const MBR& a, const MBR& b) {
    return b.minx >= a.minx &&
           b.maxx <= a.maxx &&
           b.miny >= a.miny &&
           b.maxy <= a.maxy;
}

//intersects tests a intersect b (mbr)
bool intersects(const MBR& a, const MBR& b) {
    return !(
            b.minx > a.maxx ||
            b.maxx < a.minx ||
            b.miny > a.maxy ||
            b.maxy < a.miny);
}


//computes the intersection area of two mbrs
double intersection_area(const MBR& a, const MBR& b) {
    auto minx = a.minx;
    auto miny = a.miny;
    auto maxx = a.maxx;
    auto maxy = a.maxy;

    if (!intersects(a, b)) {
        return 0.0;
    }

    if (b.minx > minx) {
        minx = b.minx;
    }

    if (b.miny > miny) {
        miny = b.miny;
    }

    if (b.maxx < maxx) {
        maxx = b.maxx;
    }

    if (b.maxy < maxy) {
        maxy = b.maxy;
    }

    return (maxx - minx) * (maxy - miny);
}


#endif //RTREE_CPP_UTIL_H
