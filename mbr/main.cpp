#include <iostream>
#include <memory>
#include <utility>
#include "mbr.h"
using namespace std;
    std::tuple<mbr::xy, mbr::xy> llur() {
        return std::tuple<mbr::xy, mbr::xy>{
                mbr::xy{1.4, 2.9}, mbr::xy{14, 29}
        };
    }

std::vector<std::vector<mbr::f64>> as_poly_array() {
    return {
            {1.4, 2.9},
            {2,   3},
            {4,   5},
            {6,   7},
            {1.4, 2.9},
    };
}

int main() {
    std::cout << mbr::feq(0.1 + 0.2, 03) << std::endl;
    auto poly = llur();
    return 0;
}