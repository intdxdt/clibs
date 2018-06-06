#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "../random/rand.h"
#include "../validate_seq/validate_seq.h"
#include "scale.h"

using namespace robust;
using af = std::vector<double>;
TEST_CASE("robust scale", "[scale]") {
    SECTION("test robust scale") {
        auto r = URandom();
            REQUIRE(scale(af{4}, 2)== af{8});
            auto expects = af{2, 2e64};
            REQUIRE(scale(af{1, 1e64}, 2)==expects);
            REQUIRE(scale(af{1}, 1)==af{1});
            auto s = scale(af{-2.4707339790384e-144, -1.6401064715739963e-142, 2e-126}, -10e-64);
            REQUIRE(s[s.size()-1] < 0);

             for (auto i = -50; i < 50; i++ ) {
                 for (auto j = -50; j < 50; j++ ) {
                    REQUIRE(scale(af{(double(i))}, double(j))==af{double(i * j)});
                }
            }
    }
}
