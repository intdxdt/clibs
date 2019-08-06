#include <cmath>
#include <vector>
#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <fstream>
#include <random>
#include "json.hpp"
#include "../mbr/mbr.h"
#include "rtree.h"


using namespace std;


std::string read_file(const std::string &fpath) {
    std::ifstream fid(fpath);
    std::stringstream buffer;
    buffer << fid.rdbuf();
    return buffer.str();
}

template<typename T>
std::vector<std::array<T, 4>> box_array(std::vector<rtree::Item<T>> items) {
    std::vector<std::array<T, 4>> boxes;
    boxes.reserve(items.size());
    for (rtree::Item<T> &item : items) {
        boxes.emplace_back(item.box.as_array());
    }
    return boxes;
}


template<typename T>
std::vector<rtree::Id> id_array(std::vector<rtree::Item<T>> items) {
    std::vector<rtree::Id> ids;
    ids.reserve(items.size());
    for (const auto &item : items) {
        ids.emplace_back(item.id);
    }
    return ids;
}

nlohmann::json json_object(std::vector<rtree::Item<double>> data) {
    nlohmann::json obj;
    obj["ids"] = id_array(data);
    obj["boxes"] = box_array(data);
    return obj;
}

std::vector<rtree::Item<double>> json_to_item_data(nlohmann::json jdata, const std::string &key) {
    auto obj = jdata[key];
    std::vector<rtree::Id> sm_ids = obj["ids"];
    std::vector<std::array<double, 4>> sm_boxes = obj["boxes"];
    std::vector<rtree::Item<double>> items;
    items.reserve(sm_ids.size());

    for (size_t i = 0; i < sm_ids.size(); i++) {
        rtree::Item<double> item = {sm_ids[i], mbr::MBR(sm_boxes[i]),};
        items.emplace_back(item);
    }
    return items;
}


void write_to_file(const std::string &fpath, const std::string &data) {
    std::ofstream fid(fpath);
    fid << data;
}


void dump_tracks_to_json_file(std::vector<rtree::Item<double>> small, std::vector<rtree::Item<double>> large,
                              const std::string &output_filename) {
    nlohmann::json obj;
    obj["small"] = json_object(small);
    obj["large"] = json_object(large);
    write_to_file(output_filename, obj.dump());
}


int main() {
    auto json_data = read_file("debug.txt");
    auto data = nlohmann::json::parse(json_data);
    auto smaller = json_to_item_data(data, "small");
    auto larger = json_to_item_data(data, "large");


    std::vector<rtree::Item<double>> cloneData(larger.begin(), larger.end());
    cloneData.insert(cloneData.end(), smaller.begin(), smaller.end());
    for (auto o : cloneData) {
        cout << o.id << '\n';
    }

    auto tree1 = rtree::NewRTree<double>(4);
    tree1.load(larger);
    tree1.load(smaller);

    auto tree2 = rtree::NewRTree<double>(4);
    tree2.load(smaller);
    tree2.load(larger);
    return 0;
}