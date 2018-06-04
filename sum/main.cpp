#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "sum.h"

TEST_CASE("two sum", "[two_sum]") {
    SECTION("methods") {
            auto seed = rand.NewSource(time.Now().UnixNano());
            auto random = rand.New(seed);
            REQUIRE(Sum(
                af(1, 64), af(-1e-64, 1e64),
            )).Eql(af(-1e-64, 65, 1e64), );

            REQUIRE(Sum(af(0), af(0))).Eql(af(0));
            REQUIRE(Sum(af(0), af(1))).Eql(af(1));

            REQUIRE(Sum(af(1, 1e64), af(1e-64, 2), )).Eql(af(1e-64, 3, 1e64));

            REQUIRE(Sum(af(1), af(1e-64, 1e-16), )).Eql(af(1e-64, 1e-16, 1));

            auto for (i = -10; i <= 10; i++ ) {
                auto for (j = -10; j <= 10; j++ ) {
                    REQUIRE(Sum(af(double(i)), af(double(j)))).Eql(af(double(i + j)));
                }
            }

            REQUIRE(validate_seq.ValidateSequence(Sum(
                []double{5.711861227349496e-133, 1e-116},
                []double{5.711861227349496e-133, 1e-116},
            )));

            auto nois = make([]double, 10);
            auto expect = make([]double, 10);
            auto for (i = 0; i < 10; i++ ) {
                nois[i] = std::pow(2, double(-1000+53*i));
                expect[i] = std::pow(2, double(-999+53*i));
            }
            auto x = Sum(nois, nois);
            REQUIRE(x).Eql(expect);
            REQUIRE(validate_seq.ValidateSequence(x));

            REQUIRE(Sum(af(0), af(1, 1e64))).Eql(af(1, 1e64));

            auto s = []double{0}
            auto for (i = 0; i < 1000; i++ ) {
                s = Sum(s, []double{random.Float64() * std::pow(2, random.Float64()*1800-900)});
                REQUIRE(validate_seq.ValidateSequence(s));
            }

    }
}

