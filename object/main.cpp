#define CATCH_CONFIG_MAIN
#include <iostream>
#include <cmath>
#include "../catch/catch.h"
#include "object.h"


TEST_CASE("object", "[object 1]") {
    SECTION("object") {
        Object obj;
        auto bbox = obj.bounds();
        REQUIRE(std::isnan(bbox[0]));
        REQUIRE(std::isnan(bbox[1]));
        REQUIRE(std::isnan(bbox[2]));
        REQUIRE(std::isnan(bbox[3]));
    }
}