#define CATCH_CONFIG_MAIN

#include <cmath>
#include <vector>
#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <random>
#include "rtree.h"
#include "../catch/catch.h"

using namespace std;
namespace rtest {
    size_t N = size_t(1e6);
    size_t maxFill = 64;
    //data from rbush 1.4.2
//@formatter:off
    std::vector<MBR> data = {{0, 0, 0, 0}, {10, 10, 10, 10}, {20, 20, 20, 20}, {25, 0, 25, 0}, {35, 10, 35, 10}, {45, 20, 45, 20}, {0, 25, 0, 25}, {10, 35, 10, 35},
                          {20, 45, 20, 45}, {25, 25, 25, 25}, {35, 35, 35, 35}, {45, 45, 45, 45}, {50, 0, 50, 0}, {60, 10, 60, 10}, {70, 20, 70, 20}, {75, 0, 75, 0},
                          {85, 10, 85, 10}, {95, 20, 95, 20}, {50, 25, 50, 25}, {60, 35, 60, 35}, {70, 45, 70, 45}, {75, 25, 75, 25}, {85, 35, 85, 35}, {95, 45, 95, 45},
                          {0, 50, 0, 50}, {10, 60, 10, 60}, {20, 70, 20, 70}, {25, 50, 25, 50}, {35, 60, 35, 60}, {45, 70, 45, 70}, {0, 75, 0, 75}, {10, 85, 10, 85},
                          {20, 95, 20, 95}, {25, 75, 25, 75}, {35, 85, 35, 85}, {45, 95, 45, 95}, {50, 50, 50, 50}, {60, 60, 60, 60}, {70, 70, 70, 70}, {75, 50, 75, 50},
                          {85, 60, 85, 60}, {95, 70, 95, 70}, {50, 75, 50, 75}, {60, 85, 60, 85}, {70, 95, 70, 95}, {75, 75, 75, 75}, {85, 85, 85, 85}, {95, 95, 95, 95}};
//@formatter:on
    std::vector<MBR> someData(size_t n) {
        std::vector<MBR> data;
        data.reserve(n);
        for (size_t i = 0; i < n; i++) {
            data.emplace_back(MBR{double(i), double(i), double(i), double(i)});
        }
        return std::move(data);
    }

    void testResults(std::vector<std::shared_ptr<rtree::Node>>&& nodes, std::vector<MBR>&& boxes) {
        std::sort(nodes.begin(), nodes.end(), rtree::xy_node_path());
        std::sort(boxes.begin(), boxes.end(), rtree::xy_boxes());
        REQUIRE(nodes.size() == boxes.size());
        for (int i = 0; i < nodes.size(); i++) {
            REQUIRE(nodes[i]->bbox.equals(boxes[i]));
        }
    }

    auto getnodes(std::vector<std::shared_ptr<rtree::Node>> nodes) {
        std::vector<rtree::Node> res0;
        for (auto& o : nodes) {
            res0.emplace_back(*o);
        }
        return res0;
    };


    bool nodeEquals(rtree::Node a, rtree::Node b) {
        auto bln = a.bbox == b.bbox;
        bln = bln &&
              a.item.id == b.item.id &&
              a.item.bbox == b.item.bbox &&
              a.item.meta == b.item.meta;
        bln = bln &&
              a.height == b.height &&
              a.leaf == b.leaf &&
              a.bbox == b.bbox &&
              a.children.size() == b.children.size();
        if (bln && !a.children.empty()) {
            for (size_t i = 0; bln && i < a.children.size(); i++) {
                bln = bln && nodeEquals(*(a.children[i]), *(b.children[i]));
            }
        }
        return bln;
    }

    auto testAssertEquals(std::vector<rtree::Node> a, std::vector<rtree::Node> b) {
        REQUIRE(a.size() == b.size());
        for (size_t i = 0; i < a.size(); i++) {
            auto oa = a[i];
            auto ob = b[i];
            REQUIRE(oa.bbox.equals(ob.bbox));
        }
        return true;
    };

    MBR RandBox(double size, const std::function<double()>& rnd) {
        auto x = rnd() * (100.0 - size);
        auto y = rnd() * (100.0 - size);
        return {x, y, x + size * rnd(), y + size * rnd()};
    }

    std::vector<MBR> GenDataItems(size_t N, double size) {
        std::vector<MBR> data;
        data.reserve(N);

        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        auto rnd = [&]() {
            return distribution(generator);
        };

        for (size_t i = 0; i < N; ++i) {
            data.emplace_back(RandBox(size, rnd));
        }
        return std::move(data);
    }


//    auto BenchData = GenDataItems(N, 1);
//    auto bboxes100 = GenDataItems(1000, 100*std::sqrt(0.1));
//    auto bboxes10 = GenDataItems(1000, 10);
//    auto bboxes1 = GenDataItems(1000, 1);
//    auto tree = NewRTree(maxFill).Load(BenchData);
}


TEST_CASE("rtree 1", "[rtree 1]") {

    using namespace rtree;
    using namespace rtest;

    SECTION("should test load 9 & 10") {
        auto tree0 = rtree::new_RTree(0).load_boxes(someData(0));
        REQUIRE(tree0.data->height == 1);

        auto tree1 = new_RTree(9).load_boxes(someData(9));
        REQUIRE(tree1.data->height == 1);

        auto tree2 = new_RTree(9).load_boxes(someData(10));
        REQUIRE(tree2.data->height == 2);
    }
    SECTION("tests search with some other") {
        vector<MBR> data{
                {-115, 45,  -105, 55},
                {105,  45,  115,  55},
                {105,  -55, 115,  -45},
                {-115, -55, -105, -45},
        };
        auto tree = new_RTree(4);
        tree.load_boxes(data);
        testResults(tree.search({-180, -90, 180, 90}), std::vector<MBR>{
                {-115, 45,  -105, 55},
                {105,  45,  115,  55},
                {105,  -55, 115,  -45},
                {-115, -55, -105, -45},
        });

        testResults(tree.search(MBR(-180, -90, 0, 90)), std::vector<MBR>{
                {-115, 45,  -105, 55},
                {-115, -55, -105, -45},
        });

        testResults(tree.search(MBR(0, -90, 180, 90)), std::vector<MBR>{
                {105, 45,  115, 55},
                {105, -55, 115, -45},
        });
        testResults(tree.search(MBR(-180, 0, 180, 90)), std::vector<MBR>{
                {-115, 45, -105, 55},
                {105,  45, 115,  55},
        });
        testResults(tree.search(MBR(-180, -90, 180, 0)), std::vector<MBR>{
                {105,  -55, 115,  -45},
                {-115, -55, -105, -45},
        });
    }

    SECTION("#load uses standard insertion when given a low number of items") {
        auto data = rtest::data;
        auto tree = new_RTree(8).load_boxes(
                rtest::data
        ).load_boxes(slice(data, 0, 3));

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
        testResults(tree1.all(), [&] { return cloneData; }());
        testResults(tree2.all(), [&] { return cloneData; }());
    }
    SECTION("#load properly merges data of smaller or bigger tree heights 2") {
        auto N = 8020ul;
        std::vector<MBR> smaller = GenDataItems(N, 1);
        std::vector<MBR> larger = GenDataItems(2 * N, 1);
        std::vector<MBR> cloneData(larger.begin(), larger.end());
        cloneData.insert(cloneData.end(), smaller.begin(), smaller.end());

        auto tree1 = new_RTree(64).load_boxes(larger).load_boxes(smaller);
        auto tree2 = new_RTree(64).load_boxes(smaller).load_boxes(larger);
        REQUIRE(tree1.data->height == tree2.data->height);
        testResults(tree1.all(), [&] { return cloneData; }());
        testResults(tree2.all(), [&] { return cloneData; }());
    }
    SECTION("#search finds matching points in the tree given a bbox") {
        auto data = rtest::data;
        auto tree = new_RTree(4).load_boxes(data);
        //@formatter:off
        testResults(tree.search(MBR(40, 20, 80, 70)), std::vector<MBR>{
                {70, 20, 70, 20}, {75, 25, 75, 25}, {45, 45, 45, 45}, {50, 50, 50, 50}, {60, 60, 60, 60}, {70, 70, 70, 70}, {45, 20, 45, 20}, {45, 70, 45, 70}, {75, 50, 75, 50}, {50, 25, 50, 25}, {60, 35, 60, 35}, {70, 45, 70, 45},
        });
        //@formatter:on
    }
    SECTION("#collides returns true when search finds matching points") {
        auto data = rtest::data;
        auto tree = new_RTree(4).load_boxes(data);
        REQUIRE(tree.collides(MBR(40, 20, 80, 70)));
        REQUIRE(!tree.collides(MBR(200, 200, 210, 210)));
    }

    SECTION("#search returns an empty array if (nothing found") {
        auto data = rtest::data;
        auto result = new_RTree(4).load_boxes(data).search(MBR(200, 200, 210, 210));
        REQUIRE(result.empty());
    }

    SECTION("#all <==>.Data returns all points in the tree") {
        auto data = rtest::data;
        std::vector<MBR> cloneData(data.begin(), data.end());
        auto tree = new_RTree(4).load_boxes(data);
        auto result = tree.search(MBR(0, 0, 100, 100));
        testResults(std::move(result), std::move(cloneData));
    }
    SECTION("#all <==>.Data returns all points in the tree") {
        std::vector<MBR> data = {{0, 0, 0, 0},
                                 {2, 2, 2, 2},
                                 {1, 1, 1, 1}};
        auto tree = new_RTree(4);
        tree.load_boxes(data);
        tree.insert(Object{3, MBR{3, 3, 3, 3}});
        REQUIRE(tree.data->leaf);
        REQUIRE(tree.data->height == 1);
        REQUIRE(tree.data->bbox.equals(MBR{0, 0, 3, 3}));
        std::vector<MBR> expects{{0, 0, 0, 0},
                                 {1, 1, 1, 1},
                                 {2, 2, 2, 2},
                                 {3, 3, 3, 3}};
        REQUIRE(tree.data->children.size() == expects.size());
        testResults([&] { return tree.data->children; }(), [&] { return expects; }());
    }

    SECTION("#insert forms a valid tree if (items are inserted one by one") {
        auto data = rtest::data;
        auto tree = new_RTree(4);
        size_t index = 0;
        for (const auto& o : data) {
            tree.insert(Object{index++, o});
        }
        auto tree2 = new_RTree(4).load_boxes(data);
        REQUIRE(tree.data->height - tree2.data->height <= 1);
        std::vector<MBR> boxes2;
        auto all2 = tree2.all();
        for (auto& o : all2) {
            boxes2.emplace_back(o->bbox);
        }
        testResults(tree.all(), std::move(boxes2));
    }

    SECTION("#remove removes items correctly") {
        auto data = rtest::data;
        auto N = len(data);
        std::vector<Object> boxes;
        boxes.reserve(N);
        for (size_t i = 0; i < N; ++i) {
            boxes.emplace_back(Object{i, data[i]});
        }
        auto tree = new_RTree(4).load(boxes);
        tree.remove(data[0]);
        tree.remove(data[1]);
        tree.remove(data[2]);

        tree.remove(boxes[N - 1]);
        tree.remove(boxes[N - 2]);
        tree.remove(boxes[N - 3]);

        std::vector<MBR> cloneData(data.begin() + 3, data.end() - 3);
        testResults(tree.all(), std::move(cloneData));
    }

    SECTION("#remove removes items correctly") {
        Object item;
        auto data = rtest::data;
        auto tree = new_RTree(0).load_boxes(data);
        auto tree2 = new_RTree(0).load_boxes(data);
        tree2.remove(MBR(13, 13, 13, 13));
        REQUIRE(nodeEquals(*tree.data, *tree2.data));
        tree2.remove(item); //not init
        REQUIRE(nodeEquals(*tree.data, *tree2.data));
    }

    SECTION("#remove brings the tree to a clear state when removing everything one by one"){
            auto data = rtest::data;
			auto tree = new_RTree(4).load_boxes(data);
			auto result = tree.search(MBR(0, 0, 100, 100));
			for (size_t i = 0; i < len(result); i++){
				tree.remove(result[i]);
			}
			REQUIRE(tree.is_empty());
    }
}
