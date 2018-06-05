#define CATCH_CONFIG_MAIN

#include "../catch/catch.h"
#include "twiddle.h"

using namespace std;

TEST_CASE("twiddle", "[twiddle]") {
    SECTION("count trailing zeros") {
        REQUIRE(count_trailing_zeros(0) == 32);
        REQUIRE(count_trailing_zeros(1) == 0);
        //    g.Assert(CountTrailingZeros(-1), 0)
        for (uint32 i = 0; i < 31; i++) {
            REQUIRE(count_trailing_zeros(1u << i) == i);
            if (i > 0) {
                REQUIRE(count_trailing_zeros((1u << i) - 1) == 0);
            }
        }
        REQUIRE(count_trailing_zeros(0xf81700) == 8);
    }

    SECTION("log2") {
			for (uint32 i = 0; i < 31; i++) {
				if (i > 0) {
					REQUIRE(log2((1u<<i)-1) == (i-1));
					REQUIRE(log2((1u<<i)+1) == i);
				}
				REQUIRE(log2(1u<<i) == i);
			}

		}
}
