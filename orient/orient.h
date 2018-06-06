//
//06/06/18.
//
#include <vector>
#include "../twoproduct/two_product.h"
#include "../sum/sum.h"
#include "../subtract/subtract.h"
#include "../scale/scale.h"

#ifndef ORIENT_ORIENT_H
#define ORIENT_ORIENT_H
namespace robust {
    const double EPSILON = 1.1102230246251565e-16;
    const double ERRBOUND3 = (3.0 + 16.0 * EPSILON) * EPSILON;
    const double ERRBOUND4 = (7.0 + 56.0 * EPSILON) * EPSILON;

//orientation 2d exact
    double
    orientation3_exact(const std::vector<double>& m0, const std::vector<double>& m1, const std::vector<double>& m2) {
        auto p = sum(
                sum(two_product(m1[1], m2[0]), two_product(-m2[1], m1[0])),
                sum(two_product(m0[1], m1[0]), two_product(-m1[1], m0[0]))
        );
        auto n = sum(two_product(m0[1], m2[0]), two_product(-m2[1], m0[0]));
        auto d = subtract(p, n);
        return d[d.size() - 1];
    }

    double
    orientation4_exact(const std::vector<double>& m0, const std::vector<double>& m1, const std::vector<double>& m2,
                       const std::vector<double>& m3) {
        auto p = sum(
                sum(
                        scale(sum(two_product(m2[1], m3[0]), two_product(-m3[1], m2[0])), m1[2]),
                        sum(
                                scale(sum(two_product(m1[1], m3[0]), two_product(-m3[1], m1[0])), -m2[2]),
                                scale(sum(two_product(m1[1], m2[0]), two_product(-m2[1], m1[0])), m3[2])
                        )
                ),
                sum(
                        scale(sum(two_product(m1[1], m3[0]), two_product(-m3[1], m1[0])), m0[2]),
                        sum(
                                scale(sum(two_product(m0[1], m3[0]), two_product(-m3[1], m0[0])), -m1[2]),
                                scale(sum(two_product(m0[1], m1[0]), two_product(-m1[1], m0[0])), m3[2])
                        )
                )
        );

        auto n = sum(
                sum(
                        scale(sum(two_product(m2[1], m3[0]), two_product(-m3[1], m2[0])), m0[2]),
                        sum(
                                scale(sum(two_product(m0[1], m3[0]), two_product(-m3[1], m0[0])), -m2[2]),
                                scale(sum(two_product(m0[1], m2[0]), two_product(-m2[1], m0[0])), m3[2])
                        )
                ),
                sum(
                        scale(sum(two_product(m1[1], m2[0]), two_product(-m2[1], m1[0])), m0[2]),
                        sum(
                                scale(sum(two_product(m0[1], m2[0]), two_product(-m2[1], m0[0])), -m1[2]),
                                scale(sum(two_product(m0[1], m1[0]), two_product(-m1[1], m0[0])), m2[2])
                        )
                )
        );
        auto d = subtract(p, n);
        return d[d.size() - 1];

    }


//orientation in 2d space
// < 0 if ccw - c is on left of segment(a, b)
// > 0 if cw - c is on right of segment(a, b)
// = 0 if a, b, and c are coplanar
    double
    orientation(const std::vector<double>& a, const std::vector<double>& b, const std::vector<double>& c) {
        auto l = (a[1] - c[1]) * (b[0] - c[0]);
        auto r = (a[0] - c[0]) * (b[1] - c[1]);
        auto det = l - r;
        double s{0};
        if (l > 0) {
            if (r <= 0) {
                return det;
            }
            else {
                s = l + r;
            }
        }
        else if (l < 0) {
            if (r >= 0) {
                return det;
            }
            else {
                s = -(l + r);
            }
        }
        else {
            return det;
        }
        auto tol = ERRBOUND3 * s;
        if (det >= tol || det <= -tol) {
            return det;
        }
        return orientation3_exact(a, b, c);
    }

    double orientation(const std::vector<double>& a, const std::vector<double>& b, const std::vector<double>& c,
                       const std::vector<double>& d) {
        auto adx = a[0] - d[0];
        auto bdx = b[0] - d[0];
        auto cdx = c[0] - d[0];
        auto ady = a[1] - d[1];
        auto bdy = b[1] - d[1];
        auto cdy = c[1] - d[1];
        auto adz = a[2] - d[2];
        auto bdz = b[2] - d[2];
        auto cdz = c[2] - d[2];
        auto bdxcdy = bdx * cdy;
        auto cdxbdy = cdx * bdy;
        auto cdxady = cdx * ady;
        auto adxcdy = adx * cdy;
        auto adxbdy = adx * bdy;
        auto bdxady = bdx * ady;

        auto det = adz * (bdxcdy - cdxbdy) +
                   bdz * (cdxady - adxcdy) + cdz * (adxbdy - bdxady);

        auto permanent = (std::abs(bdxcdy) + std::abs(cdxbdy)) * std::abs(adz) +
                         (std::abs(cdxady) + std::abs(adxcdy)) * std::abs(bdz) +
                         (std::abs(adxbdy) + std::abs(bdxady)) * std::abs(cdz);

        auto tol = ERRBOUND4 * permanent;
        if ((det > tol) || (-det > tol)) {
            return det;
        }
        return orientation4_exact(a, b, c, d);
    }

    double
    orientation_2d(const std::vector<double>& a, const std::vector<double>& b, const std::vector<double>& c) {
        return orientation(a, b, c);
    }

    double
    orientation_3d(const std::vector<double>& a, const std::vector<double>& b, const std::vector<double>& c,
                   const std::vector<double>& d) {
        return orientation(a, b, c, d);
    }

}
#endif //ORIENT_ORIENT_H
