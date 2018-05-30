#define CATCH_CONFIG_MAIN

#include <cmath>
#include <vector>
#include <iostream>
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

    void  testResults(std::vector<std::shared_ptr<rtree::Node>>&& nodes, std::vector<MBR>&& boxes) {
        std::sort(nodes.begin(), nodes.end(), rtree::xy_node_path());
        std::sort(boxes.begin(), boxes.end(), rtree::xy_boxes());
         REQUIRE(nodes.size() == boxes.size());
        for (int i =0; i < nodes.size(); i++) {
             REQUIRE(nodes[i]->bbox.equals(boxes[i]));
        }
    }

    auto getnodes (std::vector<std::shared_ptr<rtree::Node>> nodes){
        std::vector<rtree::Node> res0 ;
        for (auto& o : nodes){
            res0.emplace_back(*o);
        }
        return res0;
    };


    bool nodeEquals (rtree::Node a , rtree::Node b){
        auto bln = a.bbox == b.bbox ;
        bln = bln &&
                a.item.id   == b.item.id &&
                a.item.bbox == b.item.bbox &&
                a.item.meta == b.item.meta;
        bln = bln &&
                a.height == b.height &&
                a.leaf == b.leaf &&
                a.bbox == b.bbox &&
                a.children.size() == b.children.size();
        if (bln && !a.children.empty()) {
            for (size_t i = 0 ; bln && i < a.children.size(); i++){
                bln = bln && nodeEquals(*(a.children[i]), *(b.children[i]));
            }
        }
        return bln;
    }
    auto testAssertEquals (std::vector<rtree::Node> a , std::vector<rtree::Node> b){
        REQUIRE(a.size() == b.size());
        for (size_t i =0; i < a.size(); i++){
            auto oa = a[i];
            auto ob = b[i];
            REQUIRE(oa.bbox.equals(ob.bbox));
        }
        return true;
    };

}



TEST_CASE("rtree 1", "[rtree 1]") {

    using namespace rtree;
    using namespace rtest;

    SECTION("should test load 9 & 10") {
        auto tree0 = rtree::new_RTree(0).load_boxes(someData(0));
        REQUIRE(tree0.data->height == 1);

        auto tree1 = new_RTree(9).load_boxes(someData(9));
        REQUIRE(tree1.data->height == 1);

        auto  tree2 = new_RTree(9).load_boxes(someData(10));
        REQUIRE(tree2.data->height == 2);
    }
    SECTION("tests search with some other") {
        vector<MBR> data {
            {-115, 45, -105, 55}, {105, 45, 115, 55}, {105, -55, 115, -45}, {-115, -55, -105, -45},
        };
            auto tree = new_RTree(4);
            tree.load_boxes(data);
            testResults( tree.search({-180, -90, 180, 90}), std::vector<MBR>{
                {-115, 45, -105, 55}, {105, 45, 115, 55}, {105, -55, 115, -45}, {-115, -55, -105, -45},
            });

            testResults(tree.search(MBR(-180, -90, 0, 90)), std::vector<MBR>{
                {-115, 45, -105, 55}, {-115, -55, -105, -45},
            });

            testResults(tree.search(MBR(0, -90, 180, 90)), std::vector<MBR>{
                {105, 45, 115, 55}, {105, -55, 115, -45},
            });
            testResults(tree.search(MBR(-180, 0, 180, 90)), std::vector<MBR>{
                {-115, 45, -105, 55}, {105, 45, 115, 55},
            });
            testResults(tree.search(MBR(-180, -90, 180, 0)), std::vector<MBR>{
                {105, -55, 115, -45}, {-115, -55, -105, -45},
            });
    }

    SECTION("#load uses standard insertion when given a low number of items") {
            auto data = rtest::data;
            auto tree = new_RTree(8).load_boxes(
                rtest::data
            ).load_boxes(slice(data,0, 3));

            auto tree2 = new_RTree(8).load_boxes(rtest::data).insert(
                Object{0, data[0]}
            ).insert(Object{1, data[1]}).insert(Object{2, data[2]});
            REQUIRE(nodeEquals(*tree.data, *tree2.data));
    }
    SECTION("#load does nothing if (loading empty data)") {
            auto data = std::vector<Object>{};
            auto tree = new_RTree(0).load(data);
            REQUIRE(tree.is_empty());
    }
    SECTION("#load properly splits tree root when merging trees of the same height") {
            auto data = rtest::data;
            std::vector<MBR> cloneData(data.begin(), data.end());
            std::vector<MBR> _cloneData(data.begin(), data.end());
            cloneData.insert(cloneData.end(), _cloneData.begin(), _cloneData.end());
            auto tree = new_RTree(4).load_boxes(data).load_boxes(data);
            testResults(tree.all(), std::move(cloneData));
    }
    SECTION("#load properly merges data of smaller or bigger tree heights") {
            auto data = rtest::data;
            auto smaller = someData(10);

            std::vector<MBR> cloneData(data.begin(), data.end());
            cloneData.insert(cloneData.end(), smaller.begin(), smaller.end());

            auto tree1 = new_RTree(4).load_boxes(data).load_boxes(smaller);
            auto tree2 = new_RTree(4).load_boxes(smaller).load_boxes(data);
            REQUIRE(tree1.data->height == tree2.data->height);
            testResults(tree1.all(), [&]{return cloneData;}());
            testResults(tree2.all(), [&]{return cloneData;}());
    }
}