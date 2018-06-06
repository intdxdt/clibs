//
//05/06/18.
//
#include <vector>
#include "../twosum/two_sum.h"
#include "../twoproduct/two_product.h"

#ifndef SCALE_SCALE_H
#define SCALE_SCALE_H
namespace robust {
    std::vector<double> linear_expansion_scale(std::vector<double> e, double scale) {
        auto n = e.size();
        if (n == 1) {
            auto ts = two_product(e[0], scale);
            if (ts[0] != 0) {
                return ts;
            }
            return {ts[1]};
        }

        std::vector<double> g;
        g.resize(2 * n, 0);

        auto q = std::vector<double>{0.1, 0.1};
        auto t = std::vector<double>{0.1, 0.1};
        auto count = 0;
        q = two_product(e[0], scale);
        if (q[0] != 0) {
            g[count] = q[0];
            count++;
        }

        for (auto i = 1; i < n; i++) {
            t = two_product(e[i], scale);
            auto pq = q[1];
            q = two_sum(pq, t[0]);
            if (q[0] != 0) {
                g[count] = q[0];
                count++;
            }
            auto a = t[1];
            auto b = q[1];
            auto x = a + b;
            auto bv = x - a;
            auto y = b - bv;
            q[1] = x;
            if (y != 0) {
                g[count] = y;
                count++;
            }
        }
        if (q[1] != 0) {
            g[count] = q[1];
            count++;
        }
        if (count == 0) {
            g[count] = 0.0;
            count++;
        }
        return std::vector<double>(g.begin(), g.begin() + count);
    }

    std::vector<double> scale(const std::vector<double>& e, double scale) {
        return linear_expansion_scale(e, scale);
    }
}
#endif //SCALE_SCALE_H
