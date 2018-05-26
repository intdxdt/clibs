#include <iostream>
#include <vector>
#include "rtree.h"

using namespace std;

using ints = std::vector<int>;



int main() {
    auto box = emptyMbr();
    auto b = box.minx > box.maxx;
    auto n = rtree::newLeafNode(&box);
    auto r = rtree::NewRTree(16);
    auto nn = rtree::Node{};
    auto nnn(nn);
    std::vector<int> vals{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    cout << vals[0] << endl;

    std::function<bool(int)> predicate = [&](int i) {
        return vals[i] == 100;
    };
    auto at = sliceIndex(vals.size(), predicate);
    cout << "index at : " << at << endl;

    return 0;
}