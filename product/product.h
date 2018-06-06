//
//06/06/18.
//
#include <vector>
#include "../scale/scale.h"
#include "../sum/sum.h"

#ifndef PRODUCT_PRODUCT_H
#define PRODUCT_PRODUCT_H
namespace robust {
//Robust Product
    std::vector<double> product(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() == 1) {
            return scale(b, a[0]);
        }
        if (b.size() == 1) {
            return scale(a, b[0]);
        }
        if (a.empty() || b.empty()) {
            return std::vector<double>{0};
        }
        std::vector<double> r{0};
        if (a.size() < b.size()) {
            for (auto o : a) {
                r = sum(r, scale(b, o));
            }
        }
        else {
            for (auto o : b) {
                r = sum(r, scale(a, o));
            }
        }
        return r;
    }

}
#endif //PRODUCT_PRODUCT_H
