#define CATCH_CONFIG_MAIN

#include <iostream>
#include "../catch/catch.h"
#include "test_overlap.h"

TEST_CASE("Test Float Overlap", "[twiddle]") {
    SECTION("test double bits") {
        REQUIRE(test_overlap(1.5, 0.5));
        REQUIRE(test_overlap(std::pow(2, -52), 1.0 + std::pow(2, -52)));
        REQUIRE_FALSE(test_overlap(1.0, 0.5));
        //Test 0
        REQUIRE_FALSE(test_overlap(0.0, 1.0));
        REQUIRE_FALSE(test_overlap(0.0, 0.0));

        //test denormalized numbers
        REQUIRE_FALSE(test_overlap(std::pow(2, -1024), std::pow(2, -1023)));
        REQUIRE_FALSE(test_overlap(std::pow(2, -1023), std::pow(2, -1022)));
        REQUIRE(test_overlap(std::pow(2, -1024) + std::pow(2, -1040), std::pow(2, -1030)));
        REQUIRE_FALSE(test_overlap(std::pow(2, -1030) - std::pow(2, -1031), std::pow(2, -1030)));

    }

}
