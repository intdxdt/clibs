#include <vector>

#ifndef RBSEGS_UTIL_H
#define RBSEGS_UTIL_H
using Seg = std::vector<std::vector < double>>;
using Segs = std::vector<Seg>;

template<typename T>
inline const T& min(const T& a, const T& b) {
    return b < a ? b : a;
};

template<typename T>
inline const T& max(const T& a, const T& b) {
    return b > a ? b : a;
};
#endif //RBSEGS_UTIL_H
