#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "../random/rand.h"
#include "../validate_seq/validate_seq.h"
#include "product.h"

using namespace robust;
using af = std::vector<double>;

TEST_CASE("robust product", "[product]") {
    SECTION("test robust product") {
        auto pow2 = [](double n) {
            return std::pow(2, n);
        };
        for (auto i = -20; i <= 20; i++) {
            for (auto j = -20; j <= 20; j++) {
                double fi(i), fj(j);
                REQUIRE(product(af{fi}, af{fj}) == af{fi * fj});
            }
        }
        auto res = product(af{pow2(-50), pow2(50)}, af{pow2(-50), pow2(50)});
        auto expects = af{pow2(-100), pow2(1), pow2(100)};
        REQUIRE(res == expects);
    }
}
