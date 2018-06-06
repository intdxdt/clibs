#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "../random/rand.h"
#include "../validate_seq/validate_seq.h"
#include "det.h"

using namespace robust;
using af = std::vector<double>;
using mat = std::vector<std::vector<double>>;

TEST_CASE("robust det", "[det]") {
    SECTION("test robust determinant") {
        auto r = URandom();
        REQUIRE(det3(mat{{1, 2, 3},
                         {3, 4, 5},
                         {6, 7, 8}}) == af{0});
        REQUIRE(det2(mat{{1, 2},
                         {3, 4}}) == af{-2});
        REQUIRE(det3(mat{{1, 2, 3},
                         {3, 4, 5},
                         {6, 7, 8}}) == af{0});
        REQUIRE(det2(mat{{1, 2},
                         {3, 4}}) == af{-2});
    }
}
