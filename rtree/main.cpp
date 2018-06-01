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

    void testResults(std::vector<rtree::Node*>&& nodes, std::vector<MBR>&& boxes) {
        std::sort(nodes.begin(), nodes.end(), rtree::xy_node_path());
        std::sort(boxes.begin(), boxes.end(), rtree::xy_boxes());
        REQUIRE(nodes.size() == boxes.size());
        for (int i = 0; i < nodes.size(); i++) {
            REQUIRE(nodes[i]->bbox.equals(boxes[i]));
        }
    }


    bool nodeEquals(rtree::Node* a, rtree::Node* b) {
        auto bln = a->bbox == b->bbox;
        bln = bln &&
              a->item.id == b->item.id &&
              a->item.bbox == b->item.bbox &&
              a->item.meta == b->item.meta;
        bln = bln &&
              a->height == b->height &&
              a->leaf == b->leaf &&
              a->bbox == b->bbox &&
              a->children.size() == b->children.size();
        if (bln && !a->children.empty()) {
            for (size_t i = 0; bln && i < a->children.size(); i++) {
                bln = bln && nodeEquals(a->children[i].get(), b->children[i].get());
            }
        }
        return bln;
    }


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


    struct Pnt {
        double x;
        double y;

        MBR bbox() {
            return MBR{x, y, x + 2, y + 2};
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

    std::vector<MBR> init_knn() {
        //@formatter:off
        std::vector<MBR>  knn_data = {
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

    bool found_in(const MBR& needle, const std::vector<MBR>& haystack) {
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
        MBR bbox;
        int version;
    };

    std::vector<RichData> fn_rich_data() {
        std::vector<RichData> richData;
        std::vector<MBR> data = {
                {1,   2, 1,   2},
                {3,   3, 3,   3},
                {5,   5, 5,   5},
                {4,   2, 4,   2},
                {2,   4, 2,   4},
                {5,   3, 5,   3},
                {3,   4, 3,   4},
                {2.5, 4, 2.5, 4},
        };
        for (auto i = 0; i < data.size(); i++) {
            auto d = data[i];
            richData.emplace_back(RichData{d, i + 1});
        }
        return richData;
    }
}

TEST_CASE("rtree 1", "[rtree 1]") {

    using namespace rtree;
    using namespace rtest;

    SECTION("should test load 9 & 10") {
//        auto tree00 = rtree::NewRTree(0);
        auto tree0 = rtree::NewRTree(0).load_boxes(someData(0));
        REQUIRE(tree0.data->height == 1);

        auto tree1 = NewRTree(9).load_boxes(someData(9));
        REQUIRE(tree1.data->height == 1);

        auto tree2 = NewRTree(9).load_boxes(someData(10));
        REQUIRE(tree2.data->height == 2);
    }
    SECTION("tests search with some other") {
        vector<MBR> data{
                {-115, 45,  -105, 55},
                {105,  45,  115,  55},
                {105,  -55, 115,  -45},
                {-115, -55, -105, -45},
        };
        auto tree = NewRTree(4);
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
        auto tree = NewRTree(8).load_boxes(
                rtest::data
        ).load_boxes(slice(data, 0, 3));

        auto tree2 = NewRTree(8).load_boxes(rtest::data).insert(
                Object{0, data[0]}
        ).insert(Object{1, data[1]}).insert(Object{2, data[2]});
        REQUIRE(nodeEquals(tree.data.get(), tree2.data.get()));
    }
    SECTION("#load does nothing if (loading empty data)") {
        auto data = std::vector<Object>{};
        auto tree = NewRTree(0).load(data);
        REQUIRE(tree.is_empty());
    }
    SECTION("#load properly splits tree root when merging trees of the same height") {
        auto data = rtest::data;
        std::vector<MBR> cloneData(data.begin(), data.end());
        std::vector<MBR> _cloneData(data.begin(), data.end());
        cloneData.insert(cloneData.end(), _cloneData.begin(), _cloneData.end());
        auto tree = NewRTree(4).load_boxes(data).load_boxes(data);
        testResults(tree.all(), std::move(cloneData));
    }
    SECTION("#load properly merges data of smaller or bigger tree heights") {
        auto data = rtest::data;
        auto smaller = someData(10);

        std::vector<MBR> cloneData(data.begin(), data.end());
        cloneData.insert(cloneData.end(), smaller.begin(), smaller.end());

        auto tree1 = NewRTree(4).load_boxes(data).load_boxes(smaller);
        auto tree2 = NewRTree(4).load_boxes(smaller).load_boxes(data);
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

        auto tree1 = NewRTree(64).load_boxes(larger).load_boxes(smaller);
        auto tree2 = NewRTree(64).load_boxes(smaller).load_boxes(larger);
        REQUIRE(tree1.data->height == tree2.data->height);
        testResults(tree1.all(), [&] { return cloneData; }());
        testResults(tree2.all(), [&] { return cloneData; }());
    }
    SECTION("#search finds matching points in the tree given a bbox") {
        auto data = rtest::data;
        auto tree = NewRTree(4).load_boxes(data);
        //@formatter:off
        testResults(tree.search(MBR(40, 20, 80, 70)), std::vector<MBR>{
                {70, 20, 70, 20}, {75, 25, 75, 25}, {45, 45, 45, 45}, {50, 50, 50, 50}, {60, 60, 60, 60}, {70, 70, 70, 70}, {45, 20, 45, 20}, {45, 70, 45, 70}, {75, 50, 75, 50}, {50, 25, 50, 25}, {60, 35, 60, 35}, {70, 45, 70, 45},
        });
        //@formatter:on
    }
    SECTION("#collides returns true when search finds matching points") {
        auto data = rtest::data;
        auto tree = NewRTree(4).load_boxes(data);
        REQUIRE(tree.collides(MBR(40, 20, 80, 70)));
        REQUIRE(!tree.collides(MBR(200, 200, 210, 210)));
    }

    SECTION("#search returns an empty array if (nothing found") {
        auto data = rtest::data;
        auto result = NewRTree(4).load_boxes(data).search(MBR(200, 200, 210, 210));
        REQUIRE(result.empty());
    }

    SECTION("#all <==>.Data returns all points in the tree") {
        auto data = rtest::data;
        std::vector<MBR> cloneData(data.begin(), data.end());
        auto tree = NewRTree(4).load_boxes(data);
        auto result = tree.search(MBR(0, 0, 100, 100));
        testResults(std::move(result), std::move(cloneData));
    }
    SECTION("#all <==>.Data returns all points in the tree") {
        std::vector<MBR> data = {{0, 0, 0, 0},
                                 {2, 2, 2, 2},
                                 {1, 1, 1, 1}};
        auto tree = NewRTree(4);
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
        testResults([&] {
            std::vector<rtree::Node*> nodes;
            for (const auto& o: tree.data->children) { nodes.emplace_back(o.get()); }
            return nodes;
        }(), [&] { return expects; }());
    }

    SECTION("#insert forms a valid tree if (items are inserted one by one") {
        auto data = rtest::data;
        auto tree = NewRTree(4);
        size_t index = 0;
        for (const auto& o : data) {
            tree.insert(Object{index++, o});
        }
        auto tree2 = NewRTree(4).load_boxes(data);
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
        auto tree = NewRTree(4).load(boxes);
        tree.remove(data[0]);
        tree.remove(data[1]);
        tree.remove(data[2]);

        tree.remove(boxes[N - 1]);
        tree.remove(boxes[N - 2]);
        tree.remove(boxes[N - 3]);

        std::vector<MBR> cloneData(data.begin() + 3, data.end() - 3);
        testResults(tree.all(), std::move(cloneData));
    }

    SECTION("#remove does nothing if (nothing found)") {
        Object item;
        auto data = rtest::data;
        auto tree = NewRTree(0).load_boxes(data);
        auto tree2 = NewRTree(0).load_boxes(data);
        tree2.remove(MBR(13, 13, 13, 13));
        REQUIRE(nodeEquals(tree.data.get(), tree2.data.get()));
        tree2.remove(item); //not init
        REQUIRE(nodeEquals(tree.data.get(), tree2.data.get()));
    }

    SECTION("#remove brings the tree to a clear state when removing everything one by one") {
        auto data = rtest::data;
        auto tree = NewRTree(4).load_boxes(data);
        auto result = tree.search(MBR(0, 0, 100, 100));
        for (size_t i = 0; i < len(result); i++) {
            tree.remove(result[i]);
        }
        REQUIRE(tree.is_empty());
    }

    SECTION("#clear should clear all the data in the tree") {
        auto data = rtest::data;
        auto tree = NewRTree(4).load_boxes(data).clear();
        REQUIRE(tree.is_empty());
    }

    SECTION("should have chainable API") {
        auto data = rtest::data;
        REQUIRE(NewRTree(4)
                        .load_boxes(data)
                        .insert(Object{0, data[0]})
                        .remove(data[0])
                        .clear()
                        .is_empty());
    }
}

TEST_CASE("rtree 2", "[rtree util]") {
    using namespace rtree;
    using namespace rtest;
    SECTION("tests pop nodes") {
        auto a = NewNode(Object{0, empty_mbr()}, 0, true, std::vector<std::unique_ptr<Node>>{});
        auto b = NewNode(Object{0, empty_mbr()}, 1, true, std::vector<std::unique_ptr<Node>>{});
        auto c = NewNode(Object{0, empty_mbr()}, 1, true, std::vector<std::unique_ptr<Node>>{});
        std::vector<Node*> nodes;
        Node* n;

        n = pop(nodes);
        REQUIRE(n == nullptr);

//        nodes = {a, b, c};
        nodes.emplace_back(a.get());
        nodes.emplace_back(b.get());
        nodes.emplace_back(c.get());
        REQUIRE(len(nodes));

        n = pop(nodes);
        REQUIRE(len(nodes) == 2);
        REQUIRE(n == c.get());

        n = pop(nodes);
        REQUIRE(len(nodes) == 1);
        REQUIRE(n == b.get());

        n = pop(nodes);
        REQUIRE(len(nodes) == 0);
        REQUIRE(n == a.get());

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

        n = pop_index(indexes);
        REQUIRE(len(indexes) == 2);
        REQUIRE(n == c);

        n = pop_index(indexes);
        REQUIRE(len(indexes) == 1);
        REQUIRE(n == b);

        n = pop_index(indexes);
        REQUIRE(len(indexes) == 0);
        REQUIRE(n == a);
    }
}


TEST_CASE("rtree knn", "[rtree knn]") {
    using namespace rtree;
    using namespace rtest;
    SECTION("finds n neighbours") {
        auto knn_data = rtest::init_knn();
        auto rt = NewRTree(9).load_boxes(knn_data);
        auto nn = rt.KNN(MBR(40, 40, 40, 40), 10, [](const MBR& query, KObj obj) {
            return query.distance(obj.bbox);
        });
        //@formatter:off
        std::vector<MBR> result = {
                {38, 39, 39, 39}, {35, 39, 38, 40}, {34, 43, 36, 44}, {29, 42, 33, 42},
                {48, 38, 48, 40}, {31, 47, 33, 50}, {34, 29, 34, 32}, {29, 45, 31, 47},
                {39, 52, 39, 56}, {57, 36, 61, 40},
        };

        REQUIRE(len(nn) == len(result));
        //@formatter:on
        for (auto& n : nn) {
            REQUIRE(rtest::found_in(n.bbox, result));
        }
    }

    SECTION("finds n neighbours with geoms") {
        auto knn_data = rtest::init_knn();
        std::vector<MBR> predicate_mbr;

        auto scoreFunc = [](const MBR& query, KObj obj) {
            auto qg = query;
            auto dist = qg.distance(obj.bbox);
            return dist;
        };
        auto createPredicate = [&](double dist) {
            return [=, &predicate_mbr](KObj candidate) {
                REQUIRE(candidate.is_item);
                if (candidate.score() <= dist) {
                    predicate_mbr.emplace_back(candidate.bbox);
                    return std::tuple<bool, bool>(true, false);
                }
                return std::tuple<bool, bool>(false, true);
            };
        };
        auto rt = NewRTree(9).load_boxes(knn_data);
        auto prefFn = createPredicate(6);
        auto query = MBR(
                74.88825108886668, 82.678427498132,
                74.88825108886668, 82.678427498132
        );

        auto res = rt.KNN(query, 10, scoreFunc, prefFn);

        REQUIRE(len(res) == 2);
        for (auto i = 0; i < res.size(); i++) {
            REQUIRE(res[i].bbox == predicate_mbr[i]);
        }
    }

    SECTION("find n neighbours that do satisfy a given predicate") {
        auto knn_data = rtest::init_knn();
        auto dat = fn_rich_data();
        std::vector<Object> objects ;
        for(size_t i=0; i < dat.size(); i++){
            objects.emplace_back(Object{
                     i, dat[i].bbox, &dat[i], i
            });
        }
        auto rt = NewRTree(9).load(objects);
    }
}