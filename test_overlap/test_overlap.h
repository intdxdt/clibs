#include <cmath>
#include <vector>
#include <cstdint>
#include "../dbits/dbits.h"

#ifndef TEST_OVERLAP_TEST_OVERLAP_H
#define TEST_OVERLAP_TEST_OVERLAP_H

using int32 = int32_t;
using uint32 = uint32_t;

//use bits{count_trailing_zeros, log2};
//use db{fraction, denormalized, exponent};
int32 tz(std::vector<uint32> f) {
    if (f[0] != 0) {
        return int32(twiddle.CountTrailingZeros(f[0]));
    } else if (f[1] != 0) {
        return 32 + int32(twiddle.CountTrailingZeros(f[1]));
    }
    return 0;
}

int32 lz(std::vector<uint32> f) {
    if (f[1] != 0) {
        return 20 - int32(twiddle.Log2(f[1]));
    } else if (f[0] != 0) {
        return 52 - int32(twiddle.Log2(f[0]));
    }
    return 52;

}

int32 lo(double n) {
    auto e = exponent(n);
    auto f = fraction(n);
    std::vector<uint32> fv(f.begin(), f.end());
    auto z = tz(fv);
    return e - (52 - z);
}

int32 hi(double n) const {
    if (denormalized(n)) {
        auto v = fraction(n);
        std::vector<uint32> vv(v.begin(), v.end());
        return -(1023 + lz(vv));
    }
    return exponent(n);

}

bool TestOverlap(double a, double b) {
    if (std::abs(b) > std::abs(a)) {
        std::swap(a, b);
    }
    if (a == 0.0 || b == 0.0) {
        return false;
    }
    auto a0 = hi(a);
    auto a1 = lo(a);
    auto b0 = hi(b);
    auto b1 = lo(b);
    //[a1------a0]
    //     [b1-----b0]
    //---------or----------
    //    [a1-------a0]
    //[b1-------b0]
    return (b1 <= a0) && (a1 <= b0);
}

#endif //TEST_OVERLAP_TEST_OVERLAP_H
