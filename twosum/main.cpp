#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include "../catch/catch.h"
#include "two_sum.h"
using namespace robust;
TEST_CASE("two sum", "[two_sum]") {
    SECTION("methods") {
        std::vector<double> expects{1.0, 1.0e+64};
        REQUIRE(two_sum(1e+64, 1) == expects);
        expects = {0, 2};
        REQUIRE(two_sum(1, 1) == expects);
        expects = {0, -1415};
        REQUIRE(two_sum(0, -1415) == expects);
        expects = {1e-64, 1e64};
        REQUIRE(two_sum(1e-64, 1e64) == expects);
        expects = {0, 0};
        REQUIRE(two_sum(0, 0) == expects);
        expects = {1, 18000000000000000};
        REQUIRE(two_sum(9e15 + 1, 9e15) == expects);
    }
}