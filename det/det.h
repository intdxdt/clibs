//
//05/06/18.
//
#include <vector>

#ifndef DET_DET_H
#define DET_DET_H
namespace robust {

 std::vector<double> Det2(const std::vector<std::vector<double>>& m) {
    return Compress(
        rsum(tprod(m[0][0], m[1][1]), tprod(-m[0][1], m[1][0])),
    );
}

 std::vector<double> Det3(const std::vector<std::vector<double>>& m) const {
    return Compress(
        rsum(
            rscale(rsum(tprod(m[1][1], m[2][2]), tprod(-m[1][2], m[2][1])), m[0][0]),
            rsum(
                rscale(rsum(tprod(m[1][0], m[2][2]), tprod(-m[1][2], m[2][0])), -m[0][1]),
                rscale(rsum(tprod(m[1][0], m[2][1]), tprod(-m[1][1], m[2][0])), m[0][2]),
            ),
        ));
}

}
#endif //DET_DET_H
