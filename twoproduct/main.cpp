#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <random>
#include "../catch/catch.h"
#include "two_product.h"


namespace rtest {
    auto inf = std::numeric_limits<double>::infinity();

    std::vector<double>  init() {
        std::vector<double> testValues;
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        auto rnd = [&]() { return distribution(generator); };
        testValues = {
                0, 1,
                std::pow(2, -52),
                std::pow(2, -53),
                1.0 - std::pow(2, -53),
                1.0 + std::pow(2, -52),
                std::pow(2, -500),
                std::pow(2, 500),
                2, 0.5, 3, 1.5, 0.1, 0.3, 0.7,
        };
        for (auto i = 0; i < 50; i++) {
            testValues.emplace_back(rnd());
            testValues.emplace_back(rnd() * std::pow(2, 1000 * rnd() - 500));
        }
        for (auto i = testValues.size() - 1; i > 0; i--) {
            testValues.emplace_back(-testValues[i]);
        }
        return testValues;
    };
}

TEST_CASE("two product", "[two_product]") {
    using namespace std;
    using namespace rtest;

    SECTION("test fast two product") {
        auto testValues = init();
        Pt2D expects{std::pow(2, -104), 1.0 + std::pow(2, -51)};
        auto val = two_product(1.0 + std::pow(2, -52), 1.0 + std::pow(2, -52));
        REQUIRE(val == expects);

        for (double a : testValues) {
            REQUIRE(a * a < inf);
            expects = Pt2D{0, 0};
            REQUIRE(two_product(0, a) == expects);
            expects = Pt2D{0, a};
            REQUIRE(two_product(1, a) == expects);
            expects = Pt2D{0, -a};
            REQUIRE(two_product(-1, a) == expects);
        }
    }
}