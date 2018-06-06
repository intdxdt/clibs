//
//05/06/18.
//
#include <vector>
#include "../compress/compress.h"
#include "../twoproduct/two_product.h"
#include "../scale/scale.h"
#include "../sum/sum.h"

#ifndef DET_DET_H
#define DET_DET_H
namespace robust {
    std::vector<double> det2(const std::vector<std::vector<double>>& m) {
        return compress(sum(two_product(m[0][0], m[1][1]), two_product(-m[0][1], m[1][0])));
    }

    std::vector<double> det3(const std::vector<std::vector<double>>& m) {
        return compress(
                sum(
                        scale(sum(two_product(m[1][1], m[2][2]), two_product(-m[1][2], m[2][1])), m[0][0]),
                        sum(
                                scale(sum(two_product(m[1][0], m[2][2]), two_product(-m[1][2], m[2][0])), -m[0][1]),
                                scale(sum(two_product(m[1][0], m[2][1]), two_product(-m[1][1], m[2][0])), m[0][2])
                        )
                ));
    }

}
#endif //DET_DET_H
