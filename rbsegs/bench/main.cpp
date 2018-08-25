#include <iostream>
#include <chrono>
#include "../brute_force.h"
#include "../rbinter.h"
#include "../test_cases.h"
#include "../../mutil/mutil.h"

using namespace std;

const int NUM_ITER = 100;
const int WARM_UP = 10;
using algorFn = std::function<std::vector<std::vector<size_t>>(Segs, Segs)>;

struct Task {
    std::string name;
    algorFn algo;
};


std::vector<double> benchmark(const Segs& red, const Segs& blue, const algorFn& algo) {
    for (auto i = 0; i < WARM_UP; i++) {
        algo(red, blue);
    }

    auto start = std::chrono::steady_clock::now();
    auto count = 0;
    for (auto i = 0; i < NUM_ITER; i++) {
        auto result = algo(red, blue);
        count += result.size();
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return std::vector<double>{round((duration*1e-6) / double(NUM_ITER), 6), double(count)};
}

int main() {
    auto IntersectBruteForce = BruteForce;
    auto implementations = std::vector<Task>{
            {"Brute-force", IntersectBruteForce},
            {"RB Intersection",       RBIntersection},
    };

    init_cases();
    for (auto impl : implementations) {
        cout << "\ntesting " + impl.name + "...\n" << '\n';
        for (auto& tcase : Cases) {
            cout << "case:" + tcase.Name << '\n';
            auto res = benchmark(tcase.Red, tcase.Blue, impl.algo);
            cout << "\ttime: " << res[0] << "ms" << " - total isect: " << res[1] << '\n';
        }
    }
    return 0;
}