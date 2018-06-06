#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "../random/rand.h"
#include "../validate_seq/validate_seq.h"
#include "subtract.h"

using namespace robust;
using af = std::vector<double>;

TEST_CASE("robust subtract", "[subtract]") {
    SECTION("subtract") {
        auto r = URandom();
        REQUIRE(Subtract(af{1}, af{1}) == af{0});
        auto s = af{0};
        for (auto i = 0; i < 100; i++) {
            s = Subtract(s, af{r.random() * std::pow(2, r.random() * 1000)});
            REQUIRE(validate_sequence(s));
        }
    }
}
