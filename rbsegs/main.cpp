#define CATCH_CONFIG_MAIN

#include <cmath>
#include <vector>
#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <random>
#include "rbinter.h"
#include "../catch/catch.h"
#include "test_cases.h"
#include "brute_force.h"

using namespace std;


TEST_CASE("rbsegs 1", "[rbsegs 1]") {
    auto brutal = BruteForce;
    auto rblsi = RBIntersection;

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    auto random = [&]() {
        return distribution(generator);
    };

    SECTION("should test fuzzy") {

        for (auto j = 0; j < 10; j++) {
            Segs red;
            for (auto i = 0; i < 10 * (j + 1); i++) {
                red.emplace_back(Seg{
                        {random(), random()},
                        {random(), random()},
                });
            }


            Segs blue;
            for (auto i = 0; i < 10 * (j + 1); i++) {
                blue.emplace_back(Seg{
                        {random(), random()},
                        {random(), random()},
                });
            }
            auto expected = brutal(red, blue);
            std::sort(expected.begin(), expected.end(), lex_crossings());

            auto actual = rblsi(red, blue);
            std::sort(actual.begin(), actual.end(), lex_crossings());
            REQUIRE(actual == expected);
        };
    }

}
