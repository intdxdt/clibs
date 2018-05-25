#include <iostream>
#include "util.h"
#include "node.h"
using namespace std;

int main() {
    auto box = emptyMbr();
    auto b =  (box.minx > box.maxx);
    auto n = rtree::newLeafNode(&box);
    cout << b << endl;
    return 0;
}