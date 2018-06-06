//
//05/06/18.
//
#include <vector>
#include "../subtract/subtract.h"

#ifndef CMP_CMP_H
#define CMP_CMP_H
namespace robust {
//Robust cmp
    double cmp(const std::vector<double> &a, const std::vector<double> &b)  {
        auto d = subtract(a, b);
        return d[d.size() - 1];
    }

}
#endif //CMP_CMP_H
