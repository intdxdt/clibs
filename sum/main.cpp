#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "../random/rand.h"
#include "../validate_seq/validate_seq.h"
#include "sum.h"

using namespace robust;
using af = std::vector<double>;

TEST_CASE("robust sum", "[sum]") {
    SECTION("sum") {
        auto r = URandom();
        auto expects = af{-1e-64, 65.0, 1e64};
        REQUIRE(sum(af{1, 64}, af{-1e-64, 1e64}) == expects);

        REQUIRE(sum(af{0}, af{0}) == af{0});
        REQUIRE(sum(af{0}, af{1}) == af{1});
        expects = af{1e-64, 3., 1e64};
        REQUIRE(sum(af{1., 1e64}, af{1e-64, 2.}) == expects);
        expects = af{1e-64, 1e-16, 1.};
        REQUIRE(sum(af{1.}, af{1e-64, 1e-16}) == expects);

        for (auto i = -10; i <= 10; i++) {
            for (auto j = -10; j <= 10; j++) {
                REQUIRE(sum(af{i * 1.}, af{j * 1.}) == af{(i + j) * 1.});
            }
        }

        REQUIRE(validate_sequence(sum(
                af{5.711861227349496e-133, 1e-116},
                af{5.711861227349496e-133, 1e-116}
        )));

        std::vector<double> nois;
        nois.resize(10);
        std::vector<double> expect;
        expect.resize(10);

        for (auto i = 0; i < 10; i++) {
            nois[i] = std::pow(2, double(-1000 + 53 * i));
            expect[i] = std::pow(2, double(-999 + 53 * i));
        }
        auto x = sum(nois, nois);
        REQUIRE(x == expect);
        REQUIRE(validate_sequence(x));
        expects = af{1, 1e64};
        REQUIRE(sum(af{0}, af{1, 1e64}) == expects);

        auto s = af{0};
        for (auto i = 0; i < 1000; i++) {
            s = sum(s, af{r.random() * std::pow(2, r.random() * 1800 - 900)});
            REQUIRE(validate_sequence(s));
        }

    }
}

