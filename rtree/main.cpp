#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include "../catch/catch.h"
#include <vector>
#include "rtree.h"
#include "../catch/catch.h"

using namespace std;
//@formatter:off
namespace rtest {
    //data from rbush 1.4.2
    std::vector<MBR> data = {{0, 0, 0, 0}, {10, 10, 10, 10}, {20, 20, 20, 20}, {25, 0, 25, 0}, {35, 10, 35, 10}, {45, 20, 45, 20}, {0, 25, 0, 25}, {10, 35, 10, 35},
                          {20, 45, 20, 45}, {25, 25, 25, 25}, {35, 35, 35, 35}, {45, 45, 45, 45}, {50, 0, 50, 0}, {60, 10, 60, 10}, {70, 20, 70, 20}, {75, 0, 75, 0},
                          {85, 10, 85, 10}, {95, 20, 95, 20}, {50, 25, 50, 25}, {60, 35, 60, 35}, {70, 45, 70, 45}, {75, 25, 75, 25}, {85, 35, 85, 35}, {95, 45, 95, 45},
                          {0, 50, 0, 50}, {10, 60, 10, 60}, {20, 70, 20, 70}, {25, 50, 25, 50}, {35, 60, 35, 60}, {45, 70, 45, 70}, {0, 75, 0, 75}, {10, 85, 10, 85},
                          {20, 95, 20, 95}, {25, 75, 25, 75}, {35, 85, 35, 85}, {45, 95, 45, 95}, {50, 50, 50, 50}, {60, 60, 60, 60}, {70, 70, 70, 70}, {75, 50, 75, 50},
                          {85, 60, 85, 60}, {95, 70, 95, 70}, {50, 75, 50, 75}, {60, 85, 60, 85}, {70, 95, 70, 95}, {75, 75, 75, 75}, {85, 85, 85, 85}, {95, 95, 95, 95}};

    std::vector<MBR> someData(size_t n) {
        std::vector<MBR> data;
        data.reserve(n);
        for (size_t i = 0; i < n; i++) {
            data.emplace_back(MBR{double(i), double(i),double(i),double(i)});
        }
        return std::move(data);
    }

    void  testResults(std::vector<std::shared_ptr<rtree::Node>>& nodes, std::vector<MBR>& boxes) {
        std::sort(nodes.begin(), nodes.end(), rtree::XYNodePath());
        std::sort(boxes.begin(), boxes.end(), rtree::XYBoxes());
         REQUIRE(nodes.size() == boxes.size());
        for (int i =0; i < nodes.size(); i++) {
             REQUIRE(nodes[i]->bbox.equals(boxes[i]));
        }
    }


}



TEST_CASE("rtree 1", "[rtree 1]") {

    using namespace rtree;
    using namespace rtest;

    SECTION("should test load 9 & 10") {
        auto tree0 = rtree::NewRTree(0).LoadBoxes(someData(0));
        REQUIRE(tree0.Data->height ==1);

        auto tree1 = NewRTree(9).LoadBoxes(someData(9));
        REQUIRE(tree1.Data->height == 1);

        auto  tree2 = NewRTree(9).LoadBoxes(someData(10));
        REQUIRE(tree2.Data->height ==2);
    }
    SECTION("tests search with some other") {
        vector<MBR> data {
            {-115, 45, -105, 55},
            {105, 45, 115, 55},
            {105, -55, 115, -45},
            {-115, -55, -105, -45},
        };
    }
}