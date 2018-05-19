#define CATCH_CONFIG_MAIN

#include "../catch/catch.h"
#include "twiddle.h"

using namespace std;

TEST_CASE("twiddle", "[twiddle]") {
    //("is power of 2", "[is_pow2]")
        REQUIRE(!is_pow2(40));
        REQUIRE(is_pow2(32));
        REQUIRE(is_pow2(64));

    //"min max", "[max]")
        REQUIRE(max(32, 64) == 64);
        REQUIRE(min(32, 64) == 32);
}
