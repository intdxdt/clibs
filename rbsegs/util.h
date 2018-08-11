#include <vector>

#ifndef RBSEGS_UTIL_H
#define RBSEGS_UTIL_H
const size_t x = 0;
const size_t y = 1;

using Seg = std::vector<std::vector<double>>;
using Segs = std::vector<Seg>;

template<typename T>
inline T min(T a, T b) {
    return b < a ? b : a;
};

template<typename T>
inline T max(T a, T b) {
    return b > a ? b : a;
};
#endif //RBSEGS_UTIL_H
