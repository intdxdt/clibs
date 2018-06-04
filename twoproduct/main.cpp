#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include "../catch/catch.h"
#include "two_product.h"

TEST_CASE("two product", "[two_product]") {
    SECTION("two_product()") {
        REQUIRE(two_sum(1e+64, 1).equals(Pt2D{1.0, 1.0e+64}));
    }
}