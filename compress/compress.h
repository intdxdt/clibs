//
//05/06/18.
//
#include <vector>

#ifndef COMPRESS_COMPRESS_H
#define COMPRESS_COMPRESS_H
namespace robust {
    std::vector<double> compress_expansion(std::vector<double> e)  {
        auto m = static_cast<int>(e.size());
        auto Q = e[e.size() - 1];
        int bottom = m;
        for (auto i = m - 2; i >= 0; i--) {
            auto a = Q;
            auto b = e[i];
            Q = a + b;
            auto bv = Q - a;
            auto q = b - bv;
            if (q != 0) {
                bottom--;
                e[bottom] = Q;
                Q = q;
            }
        }
        auto top = 0;
        for (auto i = bottom; i < m; i++) {
            auto a = e[i];
            auto b = Q;
            Q = a + b;
            auto bv = Q - a;
            auto q = b - bv;
            if (q != 0) {
                e[top] = q;
                top++;
            }
        }
        e[top] = Q;
        top++;
        return std::vector<double>(e.begin(), e.begin() + top);
    }

    std::vector<double> Compress(const std::vector<double>& e) {
        return compress_expansion(e);
    }
}
#endif //COMPRESS_COMPRESS_H
