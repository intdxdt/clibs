#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include "../catch/catch.h"
#include "two_sum.h"

TEST_CASE("two sum", "[two_sum]") {
    SECTION("methods") {
        REQUIRE(two_sum(1e+64, 1).equals(Pt2D{1.0, 1.0e+64}));
        REQUIRE(two_sum(1, 1).equals(Pt2D{0, 2}));
        REQUIRE(two_sum(0, -1415).equals(Pt2D{0, -1415}));
        REQUIRE(two_sum(1e-64, 1e64).equals(Pt2D{1e-64, 1e64}));
        REQUIRE(two_sum(0, 0).equals(Pt2D{0, 0}));
        REQUIRE(two_sum(9e15 + 1, 9e15).equals(Pt2D{1, 18000000000000000}));
    }
}