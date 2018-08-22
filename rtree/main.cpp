#define CATCH_CONFIG_MAIN

#include <cmath>
#include <vector>
#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <random>
#include "../catch/catch.h"
#include "../mbr/mbr.h"
#include "rtree.h"

using namespace std;

namespace rtest {
    size_t N = size_t(1e6);
    size_t maxFill = 64;
    //@formatter:off
    std::vector<mbr::MBR> data = {{0, 0, 0, 0}, {10, 10, 10, 10}, {20, 20, 20, 20}, {25, 0, 25, 0}, {35, 10, 35, 10}, {45, 20, 45, 20}, {0, 25, 0, 25}, {10, 35, 10, 35},
                              {20, 45, 20, 45}, {25, 25, 25, 25}, {35, 35, 35, 35}, {45, 45, 45, 45}, {50, 0, 50, 0}, {60, 10, 60, 10}, {70, 20, 70, 20}, {75, 0, 75, 0},
                              {85, 10, 85, 10}, {95, 20, 95, 20}, {50, 25, 50, 25}, {60, 35, 60, 35}, {70, 45, 70, 45}, {75, 25, 75, 25}, {85, 35, 85, 35}, {95, 45, 95, 45},
                              {0, 50, 0, 50}, {10, 60, 10, 60}, {20, 70, 20, 70}, {25, 50, 25, 50}, {35, 60, 35, 60}, {45, 70, 45, 70}, {0, 75, 0, 75}, {10, 85, 10, 85},
                              {20, 95, 20, 95}, {25, 75, 25, 75}, {35, 85, 35, 85}, {45, 95, 45, 95}, {50, 50, 50, 50}, {60, 60, 60, 60}, {70, 70, 70, 70}, {75, 50, 75, 50},
                              {85, 60, 85, 60}, {95, 70, 95, 70}, {50, 75, 50, 75}, {60, 85, 60, 85}, {70, 95, 70, 95}, {75, 75, 75, 75}, {85, 85, 85, 85}, {95, 95, 95, 95}};
    //@formatter:on
    std::vector<mbr::MBR> someData(size_t n) {
        std::vector<mbr::MBR> data;
        data.reserve(n);
        for (size_t i = 0; i < n; i++) {
            data.emplace_back(mbr::MBR{double(i), double(i), double(i), double(i)});
        }
        return data;
    }

    void testResults(std::vector<rtree::Node<mbr::MBR>*>&& nodes, std::vector<mbr::MBR>&& boxes) {
        std::sort(nodes.begin(), nodes.end(), rtree::xy_node_path<rtree::Node<mbr::MBR>>());
        std::sort(boxes.begin(), boxes.end(), rtree::xy_boxes());

        REQUIRE(nodes.size() == boxes.size());
        for (size_t i = 0; i < nodes.size(); i++) {
            REQUIRE(nodes[i]->bbox.equals(boxes[i]));
        }
    }

    bool nodeEquals(rtree::Node<mbr::MBR>* a, rtree::Node<mbr::MBR>* b) {
        auto bln = a->bbox.equals(b->bbox);
        if (a->item != nullptr && b->item != nullptr) {
            bln = bln && a->item->bbox().equals(b->item->bbox());
        }

        bln = bln &&
              a->height == b->height &&
              a->leaf == b->leaf &&
              a->bbox == b->bbox &&
              a->children.size() == b->children.size();

        if (bln && !a->children.empty()) {
            for (size_t i = 0; bln && i < a->children.size(); i++) {
                bln = bln && nodeEquals(&a->children[i], &b->children[i]);
            }
        }

        return bln;
    }


    mbr::MBR RandBox(double size, const std::function<double()>& rnd) {
        auto x = rnd() * (100.0 - size);
        auto y = rnd() * (100.0 - size);
        return {x, y, x + size * rnd(), y + size * rnd()};
    }

    std::vector<mbr::MBR> GenDataItems(size_t N, double size) {
        std::vector<mbr::MBR> data;
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
        return data;
    }


    struct Pnt {
        double x;
        double y;

        mbr::MBR bbox() {
            return mbr::MBR{x, y, x + 2, y + 2};
        }

        bool operator==(const Pnt& other) {
            return x == other.x && y == other.y;
        }
    };

    // auto BenchData = GenDataItems(N, 1);
    // auto bboxes100 = GenDataItems(1000, 100*std::sqrt(0.1));
    // auto bboxes10 = GenDataItems(1000, 10);
    // auto bboxes1 = GenDataItems(1000, 1);
    // auto tree = NewRTree(maxFill).Load(BenchData);

    std::vector<mbr::MBR> init_knn() {
        //@formatter:off
        std::vector<mbr::MBR>  knn_data = {
            {87, 55, 87, 56}, {38, 13, 39, 16}, {7, 47, 8, 47}, {89, 9, 91, 12}, {4, 58, 5, 60}, {0, 11, 1, 12}, {0, 5, 0, 6}, {69, 78, 73, 78},
            {56, 77, 57, 81}, {23, 7, 24, 9}, {68, 24, 70, 26}, {31, 47, 33, 50}, {11, 13, 14, 15}, {1, 80, 1, 80}, {72, 90, 72, 91}, {59, 79, 61, 83},
            {98, 77, 101, 77}, {11, 55, 14, 56}, {98, 4, 100, 6}, {21, 54, 23, 58}, {44, 74, 48, 74}, {70, 57, 70, 61}, {32, 9, 33, 12}, {43, 87, 44, 91},
            {38, 60, 38, 60}, {62, 48, 66, 50}, {16, 87, 19, 91}, {5, 98, 9, 99}, {9, 89, 10, 90}, {89, 2, 92, 6}, {41, 95, 45, 98}, {57, 36, 61, 40},
            {50, 1, 52, 1}, {93, 87, 96, 88}, {29, 42, 33, 42}, {34, 43, 36, 44}, {41, 64, 42, 65}, {87, 3, 88, 4}, {56, 50, 56, 52}, {32, 13, 35, 15},
            {3, 8, 5, 11}, {16, 33, 18, 33}, {35, 39, 38, 40}, {74, 54, 78, 56}, {92, 87, 95, 90}, {12, 97, 16, 98}, {76, 39, 78, 40}, {16, 93, 18, 95},
            {62, 40, 64, 42}, {71, 87, 71, 88}, {60, 85, 63, 86}, {39, 52, 39, 56}, {15, 18, 19, 18}, {91, 62, 94, 63}, {10, 16, 10, 18}, {5, 86, 8, 87},
            {85, 85, 88, 86}, {44, 84, 44, 88}, {3, 94, 3, 97}, {79, 74, 81, 78}, {21, 63, 24, 66}, {16, 22, 16, 22}, {68, 97, 72, 97}, {39, 65, 42, 65},
            {51, 68, 52, 69}, {61, 38, 61, 42}, {31, 65, 31, 65}, {16, 6, 19, 6}, {66, 39, 66, 41}, {57, 32, 59, 35}, {54, 80, 58, 84}, {5, 67, 7, 71},
            {49, 96, 51, 98}, {29, 45, 31, 47}, {31, 72, 33, 74}, {94, 25, 95, 26}, {14, 7, 18, 8}, {29, 0, 31, 1}, {48, 38, 48, 40}, {34, 29, 34, 32},
            {99, 21, 100, 25}, {79, 3, 79, 4}, {87, 1, 87, 5}, {9, 77, 9, 81}, {23, 25, 25, 29}, {83, 48, 86, 51}, {79, 94, 79, 95}, {33, 95, 33, 99},
            {1, 14, 1, 14}, {33, 77, 34, 77}, {94, 56, 98, 59}, {75, 25, 78, 26}, {17, 73, 20, 74}, {11, 3, 12, 4}, {45, 12, 47, 12}, {38, 39, 39, 39},
            {99, 3, 103, 5}, {41, 92, 44, 96}, {79, 40, 79, 41}, {29, 2, 29, 4},
        };
        //@formatter:on
        return knn_data;
    }

    bool found_in(const mbr::MBR& needle, const std::vector<mbr::MBR>& haystack) {
        auto found = false;
        for (auto& hay :  haystack) {
            found = needle.equals(hay);
            if (found) {
                break;
            }
        }
        return found;
    }

    struct RichData {
        mbr::MBR box;
        int version;

        mbr::MBR& bbox() {
            return box.bbox();
        }
    };

    std::vector<RichData> fn_rich_data() {
        std::vector<RichData> richData;
        std::vector<mbr::MBR> data = {
                {1,   2, 1,   2},
                {3,   3, 3,   3},
                {5,   5, 5,   5},
                {4,   2, 4,   2},
                {2,   4, 2,   4},
                {5,   3, 5,   3},
                {3,   4, 3,   4},
                {2.5, 4, 2.5, 4},
        };
        for (size_t i = 0; i < data.size(); i++) {
            richData.emplace_back(RichData{data[i], int(i) + 1});
        }
        return richData;
    }

    struct Parent {
        std::string wkt{};
        std::vector<std::string> children{};
    };

    template<typename T>
    std::vector<Parent> print_RTree(std::unique_ptr<rtree::Node<T>>& a) {
        std::vector<Parent> tokens;
        if (a == nullptr) {
            return tokens;
        }

        std::vector<rtree::Node<T>*> stack;
        stack.reserve(a->children.size());
        stack.emplace_back(a.get());
        while (!stack.empty()) {
            auto node = stack.back();
            stack.pop_back();
            auto parent = Parent{node->bbox.wkt()};
            //adopt children on stack and let node go out of scope
            for (auto& o : node->children) {
                auto n = o.get();
                if (!n->children.empty()) {
                    stack.emplace_back(n);
                    parent.children.emplace_back(n->bbox.wkt());
                }
            }
            if (!parent.children.empty())
                tokens.emplace_back(parent);
        }
        return tokens;
    }
}

TEST_CASE("rtree 1", "[rtree 1]") {

    using namespace rtest;
    using namespace rtree;

//    SECTION("should test load 9 & 10") {
//        auto tree0 = rtree::NewRTree<mbr::MBR>(0).load_boxes(someData(0));
//        REQUIRE(tree0.data.height == 1);
//
//        auto tree1 = NewRTree<mbr::MBR>(9).load_boxes(someData(9));
//        REQUIRE(tree1.data.height == 1);
//
//        auto tree2 = NewRTree<mbr::MBR>(9).load_boxes(someData(10));
//        REQUIRE(tree2.data.height == 2);
//    }

    SECTION("tests search with some other") {
        vector<mbr::MBR> data{{-115, 45,  -105, 55},
                              {105,  45,  115,  55},
                              {105,  -55, 115,  -45},
                              {-115, -55, -105, -45},};
        auto tree = NewRTree<mbr::MBR>(4);
        tree.load_boxes(data);
        auto res = tree.search({-180, -90, 180, 90});

        testResults(std::move(res), std::vector<mbr::MBR>{
                {-115, 45,  -105, 55},
                {105,  45,  115,  55},
                {105,  -55, 115,  -45},
                {-115, -55, -105, -45},
        });

        testResults(tree.search(mbr::MBR(-180, -90, 0, 90)), std::vector<mbr::MBR>{
                {-115, 45,  -105, 55},
                {-115, -55, -105, -45},
        });

        testResults(tree.search(mbr::MBR(0, -90, 180, 90)), std::vector<mbr::MBR>{
                {105, 45,  115, 55},
                {105, -55, 115, -45},
        });

        testResults(tree.search(mbr::MBR(-180, 0, 180, 90)), std::vector<mbr::MBR>{
                {-115, 45, -105, 55},
                {105,  45, 115,  55},
        });

        testResults(tree.search(mbr::MBR(-180, -90, 180, 0)), std::vector<mbr::MBR>{
                {105,  -55, 115,  -45},
                {-115, -55, -105, -45},
        });
    }

    SECTION("#load uses standard insertion when given a low number of items") {
        auto data = rtest::data;
        auto tree = NewRTree<mbr::MBR>(8).load_boxes(
                rtest::data
        ).load_boxes(slice(data, 0, 3));

        auto tree2 = NewRTree<mbr::MBR>(8).load_boxes(rtest::data).insert(&data[0]).insert(&data[1]).insert(&data[2]);
        REQUIRE(nodeEquals(&tree.data, &tree2.data));
    }
    SECTION("#load does nothing if (loading empty data)") {
        std::vector<mbr::MBR*> data{};
        auto tree = NewRTree<mbr::MBR>(0).load(data);
        REQUIRE(tree.is_empty());
    }

    SECTION("#load properly splits tree root when merging trees of the same height") {
        auto data = rtest::data;
        std::vector<mbr::MBR> cloneData(data.begin(), data.end());
        std::vector<mbr::MBR> _cloneData(data.begin(), data.end());
        cloneData.insert(cloneData.end(), _cloneData.begin(), _cloneData.end());
        auto tree = NewRTree<mbr::MBR>(4).load_boxes(data).load_boxes(data);
        testResults(tree.all(), std::move(cloneData));
    }

    SECTION("#load properly merges data of smaller or bigger tree heights") {
        auto data = rtest::data;
        auto smaller = someData(10);

        std::vector<mbr::MBR> cloneData(data.begin(), data.end());
        cloneData.insert(cloneData.end(), smaller.begin(), smaller.end());

        auto tree1 = NewRTree<mbr::MBR>(4).load_boxes(data).load_boxes(smaller);
        auto tree2 = NewRTree<mbr::MBR>(4).load_boxes(smaller).load_boxes(data);
        REQUIRE(tree1.data.height == tree2.data.height);
        testResults(tree1.all(), [&] { return cloneData; }());
        testResults(tree2.all(), [&] { return cloneData; }());
    }

    SECTION("#load properly merges data of smaller or bigger tree heights 2") {
        auto N = 8020ul;
        std::vector<mbr::MBR> smaller = GenDataItems(N, 1);
        std::vector<mbr::MBR> larger = GenDataItems(2 * N, 1);
        std::vector<mbr::MBR> cloneData(larger.begin(), larger.end());
        cloneData.insert(cloneData.end(), smaller.begin(), smaller.end());

        auto tree1 = NewRTree<mbr::MBR>(64).load_boxes(larger).load_boxes(smaller);
        auto tree2 = NewRTree<mbr::MBR>(64).load_boxes(smaller).load_boxes(larger);
        REQUIRE(tree1.data.height == tree2.data.height);
        testResults(tree1.all(), [&] { return cloneData; }());
        testResults(tree2.all(), [&] { return cloneData; }());
    }

//    SECTION("#search finds matching points in the tree given a bbox") {
//        auto data = rtest::data;
//        auto tree = NewRTree<mbr::MBR>(4).load_boxes(data);
//        //@formatter:off
//        testResults(tree.search(mbr::MBR(40, 20, 80, 70)), std::vector<mbr::MBR>{
//                {70, 20, 70, 20}, {75, 25, 75, 25}, {45, 45, 45, 45},
//                {50, 50, 50, 50}, {60, 60, 60, 60}, {70, 70, 70, 70},
//                {45, 20, 45, 20}, {45, 70, 45, 70}, {75, 50, 75, 50},
//                {50, 25, 50, 25}, {60, 35, 60, 35}, {70, 45, 70, 45},
//        });
//        //@formatter:on
//    }
//
//
//    SECTION("#collides returns true when search finds matching points") {
//        auto data = rtest::data;
//        auto tree = NewRTree<mbr::MBR>(4).load_boxes(data);
//        REQUIRE(tree.collides(mbr::MBR(40, 20, 80, 70)));
//        REQUIRE(!tree.collides(mbr::MBR(200, 200, 210, 210)));
//    }
//
//    SECTION("#search returns an empty array if (nothing found") {
//        auto data = rtest::data;
//        auto result = NewRTree<mbr::MBR>(4).load_boxes(data).search(mbr::MBR(200, 200, 210, 210));
//        REQUIRE(result.empty());
//    }
//
//    SECTION("#all <==>.Data returns all points in the tree") {
//        auto data = rtest::data;
//        std::vector<mbr::MBR> cloneData(data.begin(), data.end());
//        auto tree = NewRTree<mbr::MBR>(4).load_boxes(data);
//        auto result = tree.search(mbr::MBR(0, 0, 100, 100));
//        testResults(std::move(result), std::move(cloneData));
//    }

//    SECTION("#all <==>.Data returns all points in the tree") {
//        std::vector<mbr::MBR> data = {{0, 0, 0, 0},
//                                      {2, 2, 2, 2},
//                                      {1, 1, 1, 1}};
//        auto tree = NewRTree<mbr::MBR>(4);
//        tree.load_boxes(data);
//        auto box3333 = mbr::MBR{3, 3, 3, 3};
//        tree.insert(&box3333);
//        REQUIRE(tree.data.leaf);
//        REQUIRE(tree.data.height == 1);
//        REQUIRE(tree.data.bbox.equals(mbr::MBR{0, 0, 3, 3}));
//        std::vector<mbr::MBR> expects{{0, 0, 0, 0},
//                                      {1, 1, 1, 1},
//                                      {2, 2, 2, 2},
//                                      {3, 3, 3, 3}};
//        REQUIRE(tree.data.children.size() == expects.size());
//        testResults(
//                [&] {
//                    std::vector<rtree::Node<mbr::MBR>*> nodes;
//                    for (const auto& o: tree.data.children) {
//                        nodes.emplace_back(o.get());
//                    }
//                    return nodes;
//                }(),
//                [&] {
//                    return expects;
//                }()
//        );
//    }
//
    SECTION("#insert forms a valid tree if (items are inserted one by one") {
        auto data = rtest::data;
        auto tree = NewRTree<mbr::MBR>(4);

        for (auto& o : data) {
            tree.insert(&o);
        }

        auto tree2 = NewRTree<mbr::MBR>(4).load_boxes(data);
        REQUIRE(tree.data.height - tree2.data.height <= 1);

        std::vector<mbr::MBR> boxes2;
        boxes2.reserve(data.size() + 4);

        auto all2 = tree2.all();
        for (auto& o : all2) {
            boxes2.emplace_back(o->bbox);
        }
        testResults(tree.all(), std::move(boxes2));
    }
//
//    SECTION("#remove removes items correctly") {
//        auto data = rtest::data;
//        auto N = len(data);
//        std::vector<mbr::MBR*> boxes;
//        boxes.reserve(N);
//        for (size_t i = 0; i < N; ++i) {
//            boxes.emplace_back(&data[i]);
//        }
//        auto tree = NewRTree<mbr::MBR>(4).load(boxes);
//        tree.remove(data[0]);
//        tree.remove(data[1]);
//        tree.remove(data[2]);
//
//        tree.remove(boxes[N - 1]);
//        tree.remove(boxes[N - 2]);
//        tree.remove(boxes[N - 3]);
//
//        std::vector<mbr::MBR> cloneData(data.begin() + 3, data.end() - 3);
//        testResults(tree.all(), std::move(cloneData));
//    }
//
//    SECTION("#remove does nothing if (nothing found)") {
//        mbr::MBR* item = nullptr;
//        auto data = rtest::data;
//        auto tree = NewRTree<mbr::MBR>(0).load_boxes(data);
//        auto tree2 = NewRTree<mbr::MBR>(0).load_boxes(data);
//
//        tree2.remove(mbr::MBR(13, 13, 13, 13));
//        REQUIRE(nodeEquals(tree.data.get(), tree2.data.get()));
//
//        tree2.remove(item); //not init
//        REQUIRE(nodeEquals(tree.data.get(), tree2.data.get()));
//    }
//
//    SECTION("#remove brings the tree to a clear state when removing everything one by one") {
//        auto data = rtest::data;
//        auto tree = NewRTree<mbr::MBR>(4).load_boxes(data);
//        auto result = tree.search(mbr::MBR(0, 0, 100, 100));
//        for (size_t i = 0; i < len(result); i++) {
//            tree.remove(result[i]);
//        }
//        REQUIRE(tree.is_empty());
//    }
//
//    SECTION("#clear should clear all the data in the tree") {
//        auto data = rtest::data;
//        auto tree = NewRTree<mbr::MBR>(4).load_boxes(data).clear();
//        REQUIRE(tree.is_empty());
//    }
//
//    SECTION("should have chainable API") {
//        auto data = rtest::data;
//        REQUIRE(
//                NewRTree<mbr::MBR>(4)
//                        .load_boxes(data)
//                        .insert(&data[0])
//                        .remove(data[0])
//                        .clear()
//                        .is_empty()
//        );
//    }
}

TEST_CASE("rtree 2", "[rtree util]") {
    using namespace rtree;
    using namespace rtest;

    SECTION("tests pop nodes") {
        auto abox = empty_mbr();
        auto bbox = empty_mbr();
        auto cbox = empty_mbr();
        auto a = NewNode<mbr::MBR>(&abox, 0, true, std::vector<Node<mbr::MBR>>{});
        auto b = NewNode<mbr::MBR>(&bbox, 1, true, std::vector<Node<mbr::MBR>>{});
        auto c = NewNode<mbr::MBR>(&cbox, 1, true, std::vector<Node<mbr::MBR>>{});
        std::vector<Node<mbr::MBR>*> nodes;
        Node<mbr::MBR>* n;

        n = pop(nodes);
        REQUIRE(n == nullptr);

        //nodes = {a, b, c};
        nodes.emplace_back(&a);
        nodes.emplace_back(&b);
        nodes.emplace_back(&c);
        REQUIRE(len(nodes));

        n = pop(nodes);
        REQUIRE(len(nodes) == 2);
        REQUIRE(n == &c);

        n = pop(nodes);
        REQUIRE(len(nodes) == 1);
        REQUIRE(n == &b);

        n = pop(nodes);
        REQUIRE(len(nodes) == 0);
        REQUIRE(n == &a);

        n = pop(nodes);
        REQUIRE(len(nodes) == 0);
        REQUIRE(n == nullptr);
    }

    SECTION("tests pop index") {
        size_t a = 0;
        size_t b = 1;
        size_t c = 2;
        std::vector<size_t> indexes{};
        size_t n = 0;

        REQUIRE(len(indexes) == 0);

        indexes = {a, b, c};
        REQUIRE(len(indexes) == 3);

        n = pop(indexes);
        REQUIRE(len(indexes) == 2);
        REQUIRE(n == c);

        n = pop(indexes);
        REQUIRE(len(indexes) == 1);
        REQUIRE(n == b);

        n = pop(indexes);
        REQUIRE(len(indexes) == 0);
        REQUIRE(n == a);
    }
}

//
//TEST_CASE("rtree knn", "[rtree knn]") {
//    using namespace rtree;
//    using namespace rtest;
//    SECTION("finds n neighbours") {
//        auto knn_data = rtest::init_knn();
//        auto rt = NewRTree<mbr::MBR>(9).load_boxes(knn_data);
//        auto nn = rt.KNN(mbr::MBR(40, 40, 40, 40), 10, [](const mbr::MBR& query, KObj<mbr::MBR> obj) {
//            return query.distance(obj.bbox);
//        });
//        //@formatter:off
//        std::vector<mbr::MBR> result = {
//                {38, 39, 39, 39}, {35, 39, 38, 40}, {34, 43, 36, 44}, {29, 42, 33, 42},
//                {48, 38, 48, 40}, {31, 47, 33, 50}, {34, 29, 34, 32}, {29, 45, 31, 47},
//                {39, 52, 39, 56}, {57, 36, 61, 40},
//        };
//
//        REQUIRE(len(nn) == len(result));
//        //@formatter:on
//        for (auto& n : nn) {
//            REQUIRE(rtest::found_in(n->bbox(), result));
//        }
//    }
//
//    SECTION("finds n neighbours with geoms") {
//        auto knn_data = rtest::init_knn();
//        std::vector<mbr::MBR> predicate_mbr;
//
//        auto scoreFunc = [](const mbr::MBR& query, KObj<mbr::MBR> obj) {
//            auto qg = query;
//            auto dist = qg.distance(obj.bbox);
//            return dist;
//        };
//        auto createPredicate = [&](double dist) {
//            return [=, &predicate_mbr](KObj<mbr::MBR> candidate) {
//                REQUIRE(candidate.is_item);
//                if (candidate.score() <= dist) {
//                    predicate_mbr.emplace_back(candidate.bbox);
//                    return std::tuple<bool, bool>(true, false);
//                }
//                return std::tuple<bool, bool>(false, true);
//            };
//        };
//        auto rt = NewRTree<mbr::MBR>(9).load_boxes(knn_data);
//        auto prefFn = createPredicate(6);
//        auto query = mbr::MBR(
//                74.88825108886668, 82.678427498132,
//                74.88825108886668, 82.678427498132
//        );
//
//        auto res = rt.KNN(query, 10, scoreFunc, prefFn);
//
//        REQUIRE(len(res) == 2);
//        for (size_t i = 0; i < res.size(); i++) {
//            REQUIRE(res[i]->bbox() == predicate_mbr[i]);
//        }
//    }
//
//    SECTION("find n neighbours that do satisfy a given predicate") {
//        auto knn_data = rtest::init_knn();
//        auto dat = fn_rich_data();
//        std::vector<RichData*> objects;
//        for (size_t i = 0; i < dat.size(); i++) {
//            objects.emplace_back(&dat[i]);
//        }
//        auto rt = NewRTree<RichData>(9).load(objects);
//
//        auto scoreFn = [](mbr::MBR query, KObj<RichData> boxer) {
//            return query.distance(boxer.bbox);
//        };
//
//        auto predicate = [](KObj<RichData> v) {
//            auto o = v.get_item();
//            return std::tuple<bool, bool>(o->version < 5, false);
//        };
//
//        auto result = rt.KNN(mbr::MBR(2, 4, 2, 4), 1, scoreFn, predicate);
//
//        REQUIRE(len(result) == 1);
//
//        auto v = result[0];
//        auto expects_mbr = mbr::MBR{3, 3, 3, 3};
//        auto expects_version = 2;
//
//        REQUIRE(v->box == expects_mbr);
//        REQUIRE(v->version == expects_version);
//    }
//}
//
//TEST_CASE("rtree build - bulkload", "[rtree build - bulkload]") {
//    using namespace rtree;
//    using namespace rtest;
//
//    SECTION("same root bounds for : bulkload & single insert ") {
//        //@formatter:off
//        std::vector<mbr::MBR> data = {
//			{30.74124324842736, 1.5394264094726768, 35.574749911400275, 8.754917282902216}, {7.381378714281472, 64.86180480586492, 19.198256264240655, 68.0987794848029}, {55.08436657887449, 73.66959671568338, 64.23351406225139, 77.82561878388375}, {60.0196123955198, 57.30699966964475, 74.83877476995968, 71.6532691707469}, {70.41627091953383, 51.438036044803454, 80.79446144066551, 55.724409001469795}, {6.483303127937942, 80.37332301675087, 6.50914529921677, 82.02059482017387}, {46.67649373819957, 64.24510021830747, 49.2622050275365, 72.2146377872009}, {13.896809634528902, 52.75698091860803, 27.3474212705194, 59.708006858014954}, {45.352809515933565, 67.57279878792961, 57.71107486286911, 80.63410132702094}, {58.12911437270156, 21.853066059797676, 72.6816258699198, 25.407156729750344}, {1.228055380876119, 71.76243208229317, 3.921389356330319, 71.81985676158466}, {24.176338710683243, 40.468612774474124, 30.028427694218617, 54.92587462821439}, {75.90272549273205, 70.78950716967577, 90.24958662679839, 73.14532201100896},
//			{81.17621599681077, 43.92908059235767, 90.4623706429688, 45.683200269169774}, {10.765947677699227, 81.39085907882142, 16.395569791144023, 89.08943214908905}, {54.98460948258535, 75.98770610541906, 63.17175560560506, 89.58032814388704}, {42.47968070466303, 70.33863394618999, 53.969718678982176, 81.12499083427267}, {56.597735425362224, 22.872881616226724, 58.02513594712652, 29.461626653458254}, {28.072656807817236, 3.648771707777917, 32.25507880635046, 14.896956422497794}, {49.07401457054004, 65.43509168217955, 50.276686480083214, 72.13126764274583}, {66.92950379018822, 7.40714495221543, 78.79495207418685, 15.349551257658238}, {70.05814537971477, 81.30351958853318, 71.64399891813584, 91.16708488214654}, {21.4511094375575, 69.72891964401825, 31.722373869482286, 80.3256272486247}, {40.232777196706415, 26.760849136982976, 52.202812069867704, 34.21206366219117}, {2.368032571076858, 16.296113490306034, 12.33270360370716, 30.694571126478845}, {9.01855144170366, 55.970132314222134, 23.827554767436514, 60.48030769802354}, {80.61271599954031, 36.74002124278151, 91.79275857224492, 46.9506194268175}, {50.34990344969663, 81.49769656350821, 63.617315842569894, 83.30755417296216}, {39.18113381327339, 62.28148778267892, 46.4815234729194, 67.41798018502531},
//			{29.998914416747247, 11.59250655284693, 33.376874697786775, 12.379204853229147}, {81.64879583058361, 25.545401825528394, 93.4343371235951, 37.16442658088167}, {38.58905494531754, 31.87745905840195, 41.7616624289497, 38.45823126735888}, {0.9178278426197698, 24.298283582889418, 13.300394793306303, 29.32894041204992}, {65.26849055356847, 81.26949067385523, 69.4019309878049, 95.14982799740329}, {41.57395146960945, 42.58630560128803, 44.74131455539111, 52.67240067840212}, {78.75491794797742, 24.519635432090283, 86.62303951191035, 27.152009252646756}, {57.413508019097335, 16.222132563535784, 64.52460425468645, 26.468580365950785}, {38.70624110521209, 63.6483778012707, 42.81587531412866, 76.69707330624905}, {45.79681150909137, 40.50191132346466, 56.183424730475984, 45.059343488954596}, {59.12908726623217, 61.8670788267583, 72.67061706103317, 74.71825120772677}, {53.530204647536515, 22.210826106446316, 56.19567351522378, 36.70783763707212}, {66.56685327399163, 41.84620000931149, 67.95502218856858, 51.90145172377749}, {13.647425280602949, 48.287305203367325, 14.605520880072303, 50.785335362500966}, {9.580714642281816, 71.82612512759374, 22.052586035203777, 78.60447881685704}, {42.52476287398914, 31.798014129805892, 47.30017532169579, 43.32042676277269}, {15.231406548475704, 20.91813524362627, 27.999049905750184, 33.12719299053375}, {68.25622304622375, 36.45344418954924, 75.12753345668115, 42.96412962336906},
//			{24.674565636296396, 61.64103736035227, 33.35950737775334, 68.17273669513995}, {27.860994552259186, 54.07784655778231, 37.454370732019164, 55.03748662118532}, {12.989350409059881, 12.850601894307912, 19.63701743062105, 24.447201165885136}, {54.351699198645946, 38.669663277102835, 62.70698234918281, 50.77799147478973}, {5.195541592506005, 27.378150434771385, 12.470640457055284, 31.42600927621769}, {50.42859019394414, 76.74400020764121, 61.43712226636309, 81.94545584300995}, {78.94947703076804, 80.53231036050055, 80.65894334965007, 80.53525709875574}, {25.444253736005553, 7.68730085456098, 31.065085510940172, 20.3498357552189}, {67.23805308545823, 13.569845282055715, 72.08492158784647, 28.386336312117162}, {73.53304448250748, 72.95399805919209, 78.88399497592506, 86.10583870486123}, {5.128991214483967, 46.433989769953975, 10.301559209436643, 47.47697754635162}, {34.345971501358505, 37.67046253655506, 46.65109226249595, 43.20929547370596}, {46.288476425780644, 83.24699351224912, 53.04617705157806, 95.25275555638714}, {2.3371253741744717, 67.38540121025542, 13.258004924360035, 67.9350571187773}, {81.50701949936798, 12.96213890790966, 90.69810567341676, 26.897004984394016}, {19.618219504752606, 35.07620582977229, 22.719692101944606, 35.682818900087824}, {12.212115116661117, 56.27156067476181, 15.934817779897248, 62.75070913000411}, {68.37555295280667, 52.219237356472945, 68.38823378366567, 63.48647393313754},
//			{30.62554452606222, 60.101485548798514, 37.063824618295754, 71.04525498101337}, {56.032005794131614, 71.80616208209968, 67.22546752158931, 83.70215276205255}, {20.14317265947747, 73.77798886182363, 34.25432987619779, 87.24104072094495}, {10.507678860183212, 66.06446404977234, 22.91945017863563, 73.50576752587352}, {26.0796380640738, 39.08543029877627, 37.497243272316375, 42.198598580655705}, {58.204665266130036, 58.20119021138755, 66.86094220293387, 61.613651791527374}, {40.43959914994069, 2.5737454435527933, 47.14440867190218, 10.136829689608973}, {81.61166337839565, 57.04686555019882, 82.13024015743876, 60.52557802686094}, {1.1438702774984308, 64.4390551345789, 1.207827079116793, 74.94606495692364}, {22.698477311365394, 31.694032934311718, 23.012351437738243, 34.826851291697004}, {58.23302290469934, 63.09245797513119, 63.89603555830784, 71.13299682623365}, {1.1209075169457285, 81.28342384198416, 2.010664217814431, 85.39246047317187}, {12.031894943077951, 47.03188640891187, 17.157531829906453, 58.84050109551066}, {25.175447117884868, 53.84501614745653, 29.018643250506607, 59.38873449198591}, {2.2848309030370015, 13.908167333298184, 9.169561431787841, 19.16049137202979}, {50.013550661499245, 78.5109200392331, 61.27884750099618, 90.82242857844415},
//			{60.35181123067779, 50.30720879159393, 66.40423614499642, 62.711248070454005}, {12.818633233242565, 80.69085735063159, 25.51374909020891, 93.22537975149076}, {13.89435574446365, 30.374627423660982, 26.014177608552792, 40.22893652344269}, {68.59949104329682, 71.57717815724429, 71.14413101711249, 81.32143731631942}, {8.759053910523154, 40.17136447593845, 22.076247428918848, 51.97034411093291}, {75.0237223114521, 10.812195153356786, 75.45859644475163, 24.680056123348074}, {37.640987086884465, 44.31736944555115, 46.79079124130418, 52.298119297002756}, {77.86465045295246, 69.74685405122065, 91.0727578759392, 81.32602647164121}, {41.571023531510896, 41.188931957868, 47.81613155473583, 53.78551712929363}, {46.21623238891625, 12.566288400974617, 60.42998852835609, 23.520076065312416}, {39.651498265328506, 13.503482197678323, 50.2456922936693, 17.970333385957133}, {22.002987425318885, 4.223514231931571, 24.39665459195155, 17.79996696134728}, {10.238509846935935, 17.775671898372956, 24.90139389081459, 30.900047607940877}, {11.945673076143192, 11.005643838128806, 14.458677679728162, 25.935774067123525}, {34.15254570484473, 32.9087837466544, 39.806374568647804, 45.792474254223166}, {1.2619249479259986, 73.38259039620652, 5.732709854315865, 82.08100065666045},
//			{68.88687814624431, 70.06499982957165, 70.86758866753506, 78.39070584782843}, {53.346140703038856, 38.61621943306142, 58.18001677406793, 46.227279405415416}, {60.91283806646173, 5.328797186659199, 70.97382774644399, 11.165367727083606},
//		};
//      //@formatter:on
//
//        size_t node_size = 0;
//        auto oneT = NewRTree<mbr::MBR>(9);
//        auto one_defT = NewRTree<mbr::MBR>(node_size);
//        auto bulkT = NewRTree<mbr::MBR>(9);
//        //one by one
//        std::vector<mbr::MBR> data_oneByone(data.begin(), data.end());
//        for (size_t i = 0; i < data_oneByone.size(); i++) {
//            oneT.insert(&data_oneByone[i]);
//        }
//
//        //fill zero size
//        for (size_t i = 0; i < data_oneByone.size(); i++) {
//            one_defT.insert(&data_oneByone[i]);
//        }
//
//        auto one_mbr = oneT.data.bbox;
//        auto one_def_mbr = one_defT.data.bbox;
//
//        //bulkload
//        std::vector<mbr::MBR> data_bulkLoad(data.begin(), data.end());
//        std::vector<mbr::MBR*> bulk_items;
//        for (size_t i = 0; i < data_bulkLoad.size(); i++) {
//            bulk_items.emplace_back(&data_bulkLoad[i]);
//        }
//        bulkT.load(bulk_items);
//        auto buk_mbr = bulkT.data.bbox;
//
//        REQUIRE(one_mbr.equals(one_def_mbr));
//        REQUIRE(one_mbr.equals(buk_mbr));
//        REQUIRE(std::abs(int(bulkT.data.height - oneT.data.height)) <= 1);
//        REQUIRE(len(bulkT.data.children) == len(oneT.data.children));
//
////        auto tokens = print_RTree(oneT.data);
////        for (auto& tok : tokens) {
////            std::cout << tok.wkt << std::endl;
////            for (auto& ch: tok.children) {
////                std::cout << "    " << ch << std::endl;
////            }
////        }
//    }
//
//    SECTION("build rtree by and remove all") {
//        //@formatter:off
//            std::vector<mbr::MBR> data ={
//                {30.74124324842736, 1.5394264094726768, 35.574749911400275, 8.754917282902216}, {7.381378714281472, 64.86180480586492, 19.198256264240655, 68.0987794848029}, {55.08436657887449, 73.66959671568338, 64.23351406225139, 77.82561878388375}, {60.0196123955198, 57.30699966964475, 74.83877476995968, 71.6532691707469}, {70.41627091953383, 51.438036044803454, 80.79446144066551, 55.724409001469795}, {6.483303127937942, 80.37332301675087, 6.50914529921677, 82.02059482017387}, {46.67649373819957, 64.24510021830747, 49.2622050275365, 72.2146377872009}, {13.896809634528902, 52.75698091860803, 27.3474212705194, 59.708006858014954}, {45.352809515933565, 67.57279878792961, 57.71107486286911, 80.63410132702094}, {58.12911437270156, 21.853066059797676, 72.6816258699198, 25.407156729750344}, {1.228055380876119, 71.76243208229317, 3.921389356330319, 71.81985676158466}, {24.176338710683243, 40.468612774474124, 30.028427694218617, 54.92587462821439}, {75.90272549273205, 70.78950716967577, 90.24958662679839, 73.14532201100896},
//                {81.17621599681077, 43.92908059235767, 90.4623706429688, 45.683200269169774}, {10.765947677699227, 81.39085907882142, 16.395569791144023, 89.08943214908905}, {54.98460948258535, 75.98770610541906, 63.17175560560506, 89.58032814388704}, {42.47968070466303, 70.33863394618999, 53.969718678982176, 81.12499083427267}, {56.597735425362224, 22.872881616226724, 58.02513594712652, 29.461626653458254}, {28.072656807817236, 3.648771707777917, 32.25507880635046, 14.896956422497794}, {49.07401457054004, 65.43509168217955, 50.276686480083214, 72.13126764274583}, {66.92950379018822, 7.40714495221543, 78.79495207418685, 15.349551257658238}, {70.05814537971477, 81.30351958853318, 71.64399891813584, 91.16708488214654}, {21.4511094375575, 69.72891964401825, 31.722373869482286, 80.3256272486247}, {40.232777196706415, 26.760849136982976, 52.202812069867704, 34.21206366219117}, {2.368032571076858, 16.296113490306034, 12.33270360370716, 30.694571126478845}, {9.01855144170366, 55.970132314222134, 23.827554767436514, 60.48030769802354}, {80.61271599954031, 36.74002124278151, 91.79275857224492, 46.9506194268175}, {50.34990344969663, 81.49769656350821, 63.617315842569894, 83.30755417296216}, {39.18113381327339, 62.28148778267892, 46.4815234729194, 67.41798018502531},
//                {29.998914416747247, 11.59250655284693, 33.376874697786775, 12.379204853229147}, {81.64879583058361, 25.545401825528394, 93.4343371235951, 37.16442658088167}, {38.58905494531754, 31.87745905840195, 41.7616624289497, 38.45823126735888}, {0.9178278426197698, 24.298283582889418, 13.300394793306303, 29.32894041204992}, {65.26849055356847, 81.26949067385523, 69.4019309878049, 95.14982799740329}, {41.57395146960945, 42.58630560128803, 44.74131455539111, 52.67240067840212}, {78.75491794797742, 24.519635432090283, 86.62303951191035, 27.152009252646756}, {57.413508019097335, 16.222132563535784, 64.52460425468645, 26.468580365950785}, {38.70624110521209, 63.6483778012707, 42.81587531412866, 76.69707330624905}, {45.79681150909137, 40.50191132346466, 56.183424730475984, 45.059343488954596}, {59.12908726623217, 61.8670788267583, 72.67061706103317, 74.71825120772677}, {53.530204647536515, 22.210826106446316, 56.19567351522378, 36.70783763707212}, {66.56685327399163, 41.84620000931149, 67.95502218856858, 51.90145172377749}, {13.647425280602949, 48.287305203367325, 14.605520880072303, 50.785335362500966}, {9.580714642281816, 71.82612512759374, 22.052586035203777, 78.60447881685704}, {42.52476287398914, 31.798014129805892, 47.30017532169579, 43.32042676277269}, {15.231406548475704, 20.91813524362627, 27.999049905750184, 33.12719299053375}, {68.25622304622375, 36.45344418954924, 75.12753345668115, 42.96412962336906},
//                {24.674565636296396, 61.64103736035227, 33.35950737775334, 68.17273669513995}, {27.860994552259186, 54.07784655778231, 37.454370732019164, 55.03748662118532}, {12.989350409059881, 12.850601894307912, 19.63701743062105, 24.447201165885136}, {54.351699198645946, 38.669663277102835, 62.70698234918281, 50.77799147478973}, {5.195541592506005, 27.378150434771385, 12.470640457055284, 31.42600927621769}, {50.42859019394414, 76.74400020764121, 61.43712226636309, 81.94545584300995}, {78.94947703076804, 80.53231036050055, 80.65894334965007, 80.53525709875574}, {25.444253736005553, 7.68730085456098, 31.065085510940172, 20.3498357552189}, {67.23805308545823, 13.569845282055715, 72.08492158784647, 28.386336312117162}, {73.53304448250748, 72.95399805919209, 78.88399497592506, 86.10583870486123}, {5.128991214483967, 46.433989769953975, 10.301559209436643, 47.47697754635162}, {34.345971501358505, 37.67046253655506, 46.65109226249595, 43.20929547370596}, {46.288476425780644, 83.24699351224912, 53.04617705157806, 95.25275555638714}, {2.3371253741744717, 67.38540121025542, 13.258004924360035, 67.9350571187773}, {81.50701949936798, 12.96213890790966, 90.69810567341676, 26.897004984394016}, {19.618219504752606, 35.07620582977229, 22.719692101944606, 35.682818900087824}, {12.212115116661117, 56.27156067476181, 15.934817779897248, 62.75070913000411}, {68.37555295280667, 52.219237356472945, 68.38823378366567, 63.48647393313754},
//                {30.62554452606222, 60.101485548798514, 37.063824618295754, 71.04525498101337}, {56.032005794131614, 71.80616208209968, 67.22546752158931, 83.70215276205255}, {20.14317265947747, 73.77798886182363, 34.25432987619779, 87.24104072094495}, {10.507678860183212, 66.06446404977234, 22.91945017863563, 73.50576752587352}, {26.0796380640738, 39.08543029877627, 37.497243272316375, 42.198598580655705}, {58.204665266130036, 58.20119021138755, 66.86094220293387, 61.613651791527374}, {40.43959914994069, 2.5737454435527933, 47.14440867190218, 10.136829689608973}, {81.61166337839565, 57.04686555019882, 82.13024015743876, 60.52557802686094}, {1.1438702774984308, 64.4390551345789, 1.207827079116793, 74.94606495692364}, {22.698477311365394, 31.694032934311718, 23.012351437738243, 34.826851291697004}, {58.23302290469934, 63.09245797513119, 63.89603555830784, 71.13299682623365}, {1.1209075169457285, 81.28342384198416, 2.010664217814431, 85.39246047317187}, {12.031894943077951, 47.03188640891187, 17.157531829906453, 58.84050109551066}, {25.175447117884868, 53.84501614745653, 29.018643250506607, 59.38873449198591}, {2.2848309030370015, 13.908167333298184, 9.169561431787841, 19.16049137202979}, {50.013550661499245, 78.5109200392331, 61.27884750099618, 90.82242857844415},
//                {60.35181123067779, 50.30720879159393, 66.40423614499642, 62.711248070454005}, {12.818633233242565, 80.69085735063159, 25.51374909020891, 93.22537975149076}, {13.89435574446365, 30.374627423660982, 26.014177608552792, 40.22893652344269}, {68.59949104329682, 71.57717815724429, 71.14413101711249, 81.32143731631942}, {8.759053910523154, 40.17136447593845, 22.076247428918848, 51.97034411093291}, {75.0237223114521, 10.812195153356786, 75.45859644475163, 24.680056123348074}, {37.640987086884465, 44.31736944555115, 46.79079124130418, 52.298119297002756}, {77.86465045295246, 69.74685405122065, 91.0727578759392, 81.32602647164121}, {41.571023531510896, 41.188931957868, 47.81613155473583, 53.78551712929363}, {46.21623238891625, 12.566288400974617, 60.42998852835609, 23.520076065312416}, {39.651498265328506, 13.503482197678323, 50.2456922936693, 17.970333385957133}, {22.002987425318885, 4.223514231931571, 24.39665459195155, 17.79996696134728}, {10.238509846935935, 17.775671898372956, 24.90139389081459, 30.900047607940877}, {11.945673076143192, 11.005643838128806, 14.458677679728162, 25.935774067123525}, {34.15254570484473, 32.9087837466544, 39.806374568647804, 45.792474254223166}, {1.2619249479259986, 73.38259039620652, 5.732709854315865, 82.08100065666045},
//                {68.88687814624431, 70.06499982957165, 70.86758866753506, 78.39070584782843}, {53.346140703038856, 38.61621943306142, 58.18001677406793, 46.227279405415416}, {60.91283806646173, 5.328797186659199, 70.97382774644399, 11.165367727083606},
//            };
//            //@formatter:on
//        mbr::MBR query = {0., 0., 100, 100};
//
//        auto tree = NewRTree<mbr::MBR>(0);
//        std::vector<mbr::MBR> data_oneByone(data.begin(), data.end());
//        for (size_t i = 0; i < data_oneByone.size(); i++) {
//            tree.insert(&data_oneByone[i]);
//        }
//
//        auto res = tree.search(query);
//        for (auto& o: res) {
//            tree.remove(o);
//        }
//        REQUIRE(tree.is_empty());
//        REQUIRE(tree.data.children.empty());
//        REQUIRE(tree.data.bbox == empty_mbr());
//    }
//
//    SECTION("search for items in tree") {
//        //@formatter:off
//        std::vector<mbr::MBR> data = {
//                {30.74124324842736,  1.5394264094726768, 35.574749911400275, 8.754917282902216}, {7.381378714281472,  64.86180480586492,  19.198256264240655, 68.0987794848029}, {55.08436657887449,  73.66959671568338,  64.23351406225139,  77.82561878388375}, {60.0196123955198,   57.30699966964475,  74.83877476995968,  71.6532691707469}, {70.41627091953383,  51.438036044803454, 80.79446144066551,  55.724409001469795}, {6.483303127937942,  80.37332301675087,  6.50914529921677,   82.02059482017387}, {46.67649373819957,  64.24510021830747,  49.2622050275365,   72.2146377872009}, {13.896809634528902, 52.75698091860803,  27.3474212705194,   59.708006858014954}, {45.352809515933565, 67.57279878792961,  57.71107486286911,  80.63410132702094}, {58.12911437270156,  21.853066059797676, 72.6816258699198,   25.407156729750344}, {1.228055380876119,  71.76243208229317,  3.921389356330319,  71.81985676158466}, {24.176338710683243, 40.468612774474124, 30.028427694218617, 54.92587462821439}, {75.90272549273205,  70.78950716967577,  90.24958662679839,  73.14532201100896}, {81.17621599681077,  43.92908059235767,  90.4623706429688,   45.683200269169774}, {10.765947677699227, 81.39085907882142,  16.395569791144023, 89.08943214908905}, {54.98460948258535,  75.98770610541906,  63.17175560560506,  89.58032814388704}, {42.47968070466303,  70.33863394618999,  53.969718678982176, 81.12499083427267}, {56.597735425362224, 22.872881616226724, 58.02513594712652,  29.461626653458254}, {28.072656807817236, 3.648771707777917,  32.25507880635046,  14.896956422497794}, {49.07401457054004,  65.43509168217955,  50.276686480083214, 72.13126764274583}, {66.92950379018822,  7.40714495221543,   78.79495207418685,  15.349551257658238}, {70.05814537971477,  81.30351958853318,  71.64399891813584,  91.16708488214654}, {21.4511094375575,   69.72891964401825,  31.722373869482286, 80.3256272486247}, {40.232777196706415, 26.760849136982976, 52.202812069867704, 34.21206366219117}, {2.368032571076858,  16.296113490306034, 12.33270360370716,  30.694571126478845}, {9.01855144170366,   55.970132314222134, 23.827554767436514, 60.48030769802354}, {80.61271599954031,  36.74002124278151,  91.79275857224492,  46.9506194268175}, {50.34990344969663,  81.49769656350821,  63.617315842569894, 83.30755417296216}, {39.18113381327339,  62.28148778267892,  46.4815234729194,   67.41798018502531}, {29.998914416747247, 11.59250655284693,  33.376874697786775, 12.379204853229147}, {81.64879583058361,  25.545401825528394, 93.4343371235951,   37.16442658088167}, {38.58905494531754,  31.87745905840195,  41.7616624289497,   38.45823126735888}, {0.9178278426197698, 24.298283582889418, 13.300394793306303, 29.32894041204992}, {65.26849055356847,  81.26949067385523,  69.4019309878049,   95.14982799740329}, {41.57395146960945,  42.58630560128803,  44.74131455539111,  52.67240067840212}, {78.75491794797742,  24.519635432090283, 86.62303951191035,  27.152009252646756}, {57.413508019097335, 16.222132563535784, 64.52460425468645,  26.468580365950785}, {38.70624110521209,  63.6483778012707,   42.81587531412866,  76.69707330624905}, {45.79681150909137,  40.50191132346466,  56.183424730475984, 45.059343488954596}, {59.12908726623217,  61.8670788267583,   72.67061706103317,  74.71825120772677}, {53.530204647536515, 22.210826106446316, 56.19567351522378,  36.70783763707212}, {66.56685327399163,  41.84620000931149,  67.95502218856858,  51.90145172377749}, {13.647425280602949, 48.287305203367325, 14.605520880072303, 50.785335362500966}, {9.580714642281816,  71.82612512759374,  22.052586035203777, 78.60447881685704}, {42.52476287398914,  31.798014129805892, 47.30017532169579,  43.32042676277269}, {15.231406548475704, 20.91813524362627,  27.999049905750184, 33.12719299053375}, {68.25622304622375,  36.45344418954924,  75.12753345668115,  42.96412962336906}, {24.674565636296396, 61.64103736035227,  33.35950737775334,  68.17273669513995}, {27.860994552259186, 54.07784655778231,  37.454370732019164, 55.03748662118532}, {12.989350409059881, 12.850601894307912, 19.63701743062105,  24.447201165885136}, {54.351699198645946, 38.669663277102835, 62.70698234918281,  50.77799147478973}, {5.195541592506005,  27.378150434771385, 12.470640457055284, 31.42600927621769}, {50.42859019394414,  76.74400020764121,  61.43712226636309,  81.94545584300995}, {78.94947703076804,  80.53231036050055,  80.65894334965007,  80.53525709875574}, {25.444253736005553, 7.68730085456098,   31.065085510940172, 20.3498357552189}, {67.23805308545823,  13.569845282055715, 72.08492158784647,  28.386336312117162}, {73.53304448250748,  72.95399805919209,  78.88399497592506,  86.10583870486123}, {5.128991214483967,  46.433989769953975, 10.301559209436643, 47.47697754635162}, {34.345971501358505, 37.67046253655506,  46.65109226249595,  43.20929547370596}, {46.288476425780644, 83.24699351224912,  53.04617705157806,  95.25275555638714}, {2.3371253741744717, 67.38540121025542,  13.258004924360035, 67.9350571187773}, {81.50701949936798,  12.96213890790966,  90.69810567341676,  26.897004984394016}, {19.618219504752606, 35.07620582977229,  22.719692101944606, 35.682818900087824}, {12.212115116661117, 56.27156067476181,  15.934817779897248, 62.75070913000411}, {68.37555295280667,  52.219237356472945, 68.38823378366567,  63.48647393313754}, {30.62554452606222,  60.101485548798514, 37.063824618295754, 71.04525498101337}, {56.032005794131614, 71.80616208209968,  67.22546752158931,  83.70215276205255}, {20.14317265947747,  73.77798886182363,  34.25432987619779,  87.24104072094495}, {10.507678860183212, 66.06446404977234,  22.91945017863563,  73.50576752587352}, {26.0796380640738,   39.08543029877627,  37.497243272316375, 42.198598580655705}, {58.204665266130036, 58.20119021138755,  66.86094220293387,  61.613651791527374}, {40.43959914994069,  2.5737454435527933, 47.14440867190218,  10.136829689608973}, {81.61166337839565,  57.04686555019882,  82.13024015743876,  60.52557802686094}, {1.1438702774984308, 64.4390551345789,   1.207827079116793,  74.94606495692364}, {22.698477311365394, 31.694032934311718, 23.012351437738243, 34.826851291697004}, {58.23302290469934,  63.09245797513119,  63.89603555830784,  71.13299682623365}, {1.1209075169457285, 81.28342384198416,  2.010664217814431,  85.39246047317187}, {12.031894943077951, 47.03188640891187,  17.157531829906453, 58.84050109551066}, {25.175447117884868, 53.84501614745653,  29.018643250506607, 59.38873449198591}, {2.2848309030370015, 13.908167333298184, 9.169561431787841,  19.16049137202979}, {50.013550661499245, 78.5109200392331,   61.27884750099618,  90.82242857844415}, {60.35181123067779,  50.30720879159393,  66.40423614499642,  62.711248070454005}, {12.818633233242565, 80.69085735063159,  25.51374909020891,  93.22537975149076}, {13.89435574446365,  30.374627423660982, 26.014177608552792, 40.22893652344269}, {68.59949104329682,  71.57717815724429,  71.14413101711249,  81.32143731631942}, {8.759053910523154,  40.17136447593845,  22.076247428918848, 51.97034411093291}, {75.0237223114521,   10.812195153356786, 75.45859644475163,  24.680056123348074}, {37.640987086884465, 44.31736944555115,  46.79079124130418,  52.298119297002756}, {77.86465045295246,  69.74685405122065,  91.0727578759392,   81.32602647164121}, {41.571023531510896, 41.188931957868,    47.81613155473583,  53.78551712929363}, {46.21623238891625,  12.566288400974617, 60.42998852835609,  23.520076065312416}, {39.651498265328506, 13.503482197678323, 50.2456922936693,   17.970333385957133}, {22.002987425318885, 4.223514231931571,  24.39665459195155,  17.79996696134728}, {10.238509846935935, 17.775671898372956, 24.90139389081459,  30.900047607940877}, {11.945673076143192, 11.005643838128806, 14.458677679728162, 25.935774067123525}, {34.15254570484473,  32.9087837466544,   39.806374568647804, 45.792474254223166}, {1.2619249479259986, 73.38259039620652,  5.732709854315865,  82.08100065666045}, {68.88687814624431,  70.06499982957165,  70.86758866753506,  78.39070584782843}, {53.346140703038856, 38.61621943306142,  58.18001677406793,  46.227279405415416}, {60.91283806646173,  5.328797186659199,  70.97382774644399,  11.165367727083606},
//        };
//        //@formatter:on
//        //queries
//        //nothing
//        auto query1 = mbr::MBR{81.59858271428983, 88.95212575682031, 87.00714129337072, 92.42905627194374};
//        auto query2 = mbr::MBR{82.17807113347706, 83.15724156494792, 87.39346690616222, 84.70254401611389};
//        auto query3 = mbr::MBR{84.10969919743454, 72.14696160039038, 86.23449006778775, 79.10082263063724};
//        auto query4 = mbr::MBR{21.298871774427138, 1.1709155631470283, 36.23985259304277, 20.747325333798532};
//        auto query5 = mbr::MBR{0., 0., 100, 100};
//        auto query6 = mbr::MBR{182.17619056720642, 15.748541593521262, 205.43811579298725, 65.97783146157896};
//
//        auto tree = NewRTree<mbr::MBR>(9);
//        auto bulk_tree = NewRTree<mbr::MBR>(9);
//
//        std::vector<mbr::MBR> data_oneByone(data.begin(), data.end());
//        std::vector<mbr::MBR> data_bulkLoad(data.begin(), data.end());
//        std::vector<mbr::MBR*> bulk_items;
//        for (size_t i = 0; i < data_bulkLoad.size(); i++) {
//            bulk_items.emplace_back(&data_bulkLoad[i]);
//        }
//        bulk_tree.load(bulk_items);
//
//        for (size_t i = 0; i < data_oneByone.size(); i++) {
//            tree.insert(&data_oneByone[i]);
//        }
//        auto res1 = tree.search(query1);
//        auto res2 = tree.search(query2);
//        auto res3 = tree.search(query3);
//        auto res4 = tree.search(query4);
//        auto res5 = tree.search(query5);
//        auto res6 = tree.search(query6);
//
//        REQUIRE(len(res1) == 0);
//        REQUIRE(len(res2) == 0);
//        REQUIRE(len(res3) == 2);
//        REQUIRE(len(res4) == 6);
//        REQUIRE(len(res5) == len(data));
//        REQUIRE(len(res6) == 0);
//        REQUIRE(len(tree.all()) == len(data));
//    }
//}
