#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "../validate_seq/validate_seq.h"
#include "orient.h"

using namespace robust;
using af = std::vector<double>;

TEST_CASE("robust orient", "[orient]") {
    SECTION("test robust orient 2D") {

        REQUIRE(orientation(af{0.1, 0.1}, af{0.1, 0.1}, af{0.3, 0.7}) == 0);
        REQUIRE(orientation(af{0, 0}, af{-1e-64, 0}, af{0, 1}) > 0);

        REQUIRE(orientation_2d(af{0, 0}, af{1e-64, 1e-64}, af{1, 1}) == 0);
        REQUIRE(orientation(af{0, 0}, af{1e-64, 1e-64}, af{1, 1}) == 0);
        REQUIRE(orientation_2d(af{0, 0}, af{1e-64, 0}, af{0, 1}) < 0);
        REQUIRE(orientation(af{0, 0}, af{1e-64, 0}, af{0, 1}) < 0);

        auto x = 1e-64;
        for (auto i = 0; i < 200; i++) {
            REQUIRE(orientation(af{-x, 0}, af{0, 1}, af{x, 0}) > 0);
            REQUIRE(orientation(af{-x, 0}, af{0, 0}, af{x, 0}) == 0);
            REQUIRE(orientation(af{-x, 0}, af{0, -1}, af{x, 0}) < 0);
            REQUIRE(orientation_2d(af{-x, 0}, af{0, -1}, af{x, 0}) < 0);
            REQUIRE(orientation(af{0, 1}, af{0, 0}, af{x, x}) < 0);
            REQUIRE(orientation_2d(af{0, 1}, af{0, 0}, af{x, x}) < 0);
            x *= 10;
        }
    }

    SECTION("test robust orient 3D") {
        REQUIRE(orientation(af{0, 0, 0}, af{1, 0, 0}, af{0, 1, 0}, af{0, 0, 1}) < 0);
        REQUIRE(orientation_3d(af{0, 0, 0}, af{1, 0, 0}, af{0, 1, 0}, af{0, 0, 1}) < 0);
        REQUIRE(orientation(af{0, 0, 0}, af{1, 0, 0}, af{0, 0, 1}, af{0, 1, 0}) > 0);
        REQUIRE(orientation_3d(af{0, 0, 0}, af{1, 0, 0}, af{0, 0, 1}, af{0, 1, 0}) > 0);
        REQUIRE(orientation(af{0, 0, 0}, af{1e-64, 0, 0}, af{0, 0, 1}, af{0, 1e64, 0}) > 0);
        REQUIRE(orientation_3d(af{0, 0, 0}, af{1e-64, 0, 0}, af{0, 0, 1}, af{0, 1e64, 0}) > 0);
    }
}
