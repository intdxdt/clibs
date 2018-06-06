#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include <vector>
#include "../catch/catch.h"
#include "../random/rand.h"
#include "../validate_seq/validate_seq.h"
#include "../scale/scale.h"
#include "compress.h"

using namespace robust;
using af = std::vector<double>;

TEST_CASE("robust compress", "[compress]") {
    SECTION("test robust compress") {
        auto r = URandom();
        REQUIRE(Compress(af{0}) == af{0});
        REQUIRE(Compress(af{1}) == af{1});

        for (auto i = 0; i < 10; i++) {
            auto h = r.random();
            REQUIRE(Compress(af{h}) == af{h});
            h = -h;
            REQUIRE(Compress(af{h}) == af{h});
        }

        REQUIRE(Compress(af{1, 2}) == af{3});
        REQUIRE(Compress(af{std::pow(2, -52), 1}) == af{1 + std::pow(2, -52)});

        auto verify = [](std::vector<double> seq) {
            auto rseq = Compress(seq);
            //must Compress:  + rseq.length +  <=  + seq.length
            REQUIRE(rseq.size() <= seq.size());
            //t.same(toFixed(rseq).toString(16), toFixed(seq).toString(16), "verifying sequence")
        };
        //Bigger sequences
        for (auto i = 0; i < 10; i++) {
            std::vector<double> seq;
            for (auto j = 0; j < 18; j++) {
                seq.emplace_back(std::pow(2, -900 + 100 * double(j)) * (r.random() - 0.5));
            }
            verify(seq);
        }

        //Multiply a bunch of random numbers
        for (auto i = 0; i < 10; i++) {
            std::vector<double> seq{1};
            for (auto j = 0; j < 20; j++) {
                seq = scale(seq, 2 * r.random() - 1.0);
            }
            verify(seq);
        }
    }
}
