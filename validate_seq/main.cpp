#include <iostream>
#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "validate_seq.h"

TEST_CASE("Validate Non-Overlaping Seq", "[validate]") {
    SECTION("test validate seq") {
			REQUIRE(validate_sequence(std::vector<double>{1e-16, 1.}));
			REQUIRE_FALSE(validate_sequence(std::vector<double>{0.5, 1.5}));
			REQUIRE(validate_sequence(std::vector<double>{0.}));
			REQUIRE_FALSE(validate_sequence(std::vector<double>{}));
    }
}