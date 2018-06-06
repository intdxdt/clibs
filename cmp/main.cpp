#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "../random/rand.h"
#include "../validate_seq/validate_seq.h"
#include "cmp.h"

using namespace robust;
using af = std::vector<double>;

TEST_CASE("robust cmp", "[cmp]") {
    SECTION("test robust cmp") {
        auto r = URandom();
              REQUIRE(cmp(af{5}, af{1, 4}) == 0);
              REQUIRE(cmp(af{1e64}, af{-1e-100, 1e64}) > 0);
              REQUIRE(cmp(af{1e64}, af{1e-100, 1e64}) < 0);
    }
}
