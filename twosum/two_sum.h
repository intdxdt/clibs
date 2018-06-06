#include "../pt/pt.h"

#ifndef TWOSUM_TWOSUM_H
#define TWOSUM_TWOSUM_H
namespace robust {
///fast two sum
    inline std::vector<double> two_sum(double a, double b) {
        auto x = a + b;
        auto bv = x - a;
        auto av = x - bv;
        auto br = b - bv;
        auto ar = a - av;
        return {ar + br, x};
    }
}
#endif //TWOSUM_TWOSUM_H
