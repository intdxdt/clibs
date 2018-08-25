#include <vector>
#include "../pt/pt.h"
#ifndef RBSEGS_UTIL_H
#define RBSEGS_UTIL_H

struct Seg {
    Pt a;
    Pt b;
};

template<typename T>
inline T min(T a, T b) {
    return b < a ? b : a;
};

template<typename T>
inline T max(T a, T b) {
    return b > a ? b : a;
};
#endif //RBSEGS_UTIL_H
