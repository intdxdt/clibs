#include <iostream>
#include "rtree.h"
using namespace std;

int main() {
    auto box = emptyMbr();
    auto b =  (box.minx > box.maxx);
    auto n = rtree::newLeafNode(&box);
    auto r = rtree::NewRTree(16);
    cout << b << endl;
    return 0;
}