#include <iostream>
#include <vector>
#include "rtree.h"

using namespace std;

using ints = std::vector<int>;

//split at index

template<typename T>
std::vector<T> split_at_index(std::vector<T>& nodes, std::size_t index) {
    std::vector<T> newNodes(nodes.begin() + index, nodes.end());
    nodes.resize(index);
    return std::move(newNodes);
}

int main() {
    auto box = emptyMbr();
    auto b = (box.minx > box.maxx);
    auto n = rtree::newLeafNode(&box);
    auto r = rtree::NewRTree(16);
    auto nn = rtree::Node{};
    auto nnn(nn);
    cout << b << endl;
    std::vector<int> vals{0,1,2,3,4,5,6,7,8,9};
    cout << vals[0] << endl;
    auto from6 = split_at_index(vals, 8);

    std::cout << "myvector contains:\n";
    for (int i = 0; i < vals.size(); i++)
        std::cout << ' ' << vals[i];
    std::cout << '\n';
    for (int i = 0; i < from6.size(); i++)
        std::cout << ' ' << from6[i];
    std::cout << '\n';

    return 0;
}