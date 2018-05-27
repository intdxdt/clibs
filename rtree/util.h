#include <cmath>
#include <limits>
#include <algorithm>
#include "../mbr/mbr.h"

#ifndef RTREE_CPP_UTIL_PROTOS
#define RTREE_CPP_UTIL_PROTOS
//@formatter:off
MBR emptyMbr();
struct Object;
double bboxArea(const MBR&);
MBR& extend(MBR&, const MBR&);
double bboxMargin(const MBR&);
std::array<double, 4> emptyBounds();
bool contains(const MBR& , const MBR&) ;
bool intersects(const MBR&, const MBR&);
double enlargedArea(const MBR&, const MBR&);
double intersectionArea(const MBR& , const MBR& );
int sliceIndex(std::size_t, const std::function<bool(int)>&);
void swapItem(std::vector<Object>&, std::size_t, std::size_t);
//@formatter:on
#endif

#ifndef RTREE_CPP_OBJECT_H
#define RTREE_CPP_OBJECT_H

struct Object {
    size_t id = 0;
    MBR bbox = emptyMbr();
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
T min(T a, T b) {
    return b < a ? b : a;
};

template<typename T>
T max(T a, T b) {
    return b > a ? b : a;
};


template<typename T>
T pop(std::vector<T>& a) {
    if (a.empty()) {
        return nullptr;
    }
    auto v = a.back();
    a.resize(a.size() - 1);
    return v;
}

template<typename T>
std::vector<T> slice(const std::vector<T>& v, size_t i = 0, size_t j = 0) {
    std::vector<T> s(v.begin() + i, v.begin() + j);
    return std::move(s);
}

void swapItem(std::vector<Object>& arr, std::size_t i, std::size_t j) {
    std::swap(arr[i], arr[j]);
}

std::array<double, 4> emptyBounds() {
    auto inf = std::numeric_limits<double>::infinity();
    auto neginf = -std::numeric_limits<double>::infinity();
    return {inf, inf, neginf, neginf};
}

MBR emptyMbr() {
    return {emptyBounds(), true};
}


//slice index
int sliceIndex(std::size_t limit, const std::function<bool(int)>& predicate) {
    for (auto i = 0; i < limit; ++i) {
        if (predicate(i)) return i;
    }
    return -1;
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
double bboxArea(const MBR& a) {
    return (a.maxx - a.minx) * (a.maxy - a.miny);
}

//computes box margin
double bboxMargin(const MBR& a) {
    return (a.maxx - a.minx) + (a.maxy - a.miny);
}


//computes enlarged area given two mbrs
double enlargedArea(const MBR& a, const MBR& b) {
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
double intersectionArea(const MBR& a, const MBR& b) {
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
