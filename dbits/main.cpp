#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include "../catch/catch.h"
#include "dbits.h"

TEST_CASE("db bits", "[dbits]") {
    SECTION("construction") {
        REQUIRE(low(1.0) == 0);
        REQUIRE(high(1.0) == 0x3ff00000);
        REQUIRE(pack(0, 0x3ff00000) == 1.0);
        auto arr = std::array<uint32, 2>{0, 0x3ff00000};
        REQUIRE(double_bits(1.0) == arr);

        arr = std::array<uint32, 2>{0, 1u << 20u};
        REQUIRE(fraction(1.0) == arr);
        REQUIRE(exponent(1.0) == 0);
        REQUIRE(sign(1.0) == 0);
        REQUIRE(sign(-1.0) == 1);
        REQUIRE(exponent(0.5) == -1);

        REQUIRE(denormalized(std::pow(2, -1024)));
        REQUIRE(!denormalized(1.0));//== false
        REQUIRE(denormalized(std::pow(2, -1023)));
        REQUIRE(!denormalized(std::pow(2, -1022)));//== false
    }
}