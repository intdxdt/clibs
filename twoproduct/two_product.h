#include "../pt/pt.h"

#ifndef TWOPRODUCT_TWOPRODUCT_H
#define TWOPRODUCT_TWOPRODUCT_H
namespace robust {
    const double SPLITTER = 134217729.0;//math.Pow(2, 27) + 1.0

//two product
    std::vector<double> two_product(double a, double b) {
        auto x = a * b;

        auto c = SPLITTER * a;
        auto abig = c - a;
        auto ahi = c - abig;
        auto alo = a - ahi;

        auto d = SPLITTER * b;
        auto bbig = d - b;
        auto bhi = d - bbig;
        auto blo = b - bhi;

        auto err1 = x - (ahi * bhi);
        auto err2 = err1 - (alo * bhi);
        auto err3 = err2 - (ahi * blo);

        auto y = alo * blo - err3;

        return {y, x};
    }
}
#endif //TWOPRODUCT_TWOPRODUCT_H
