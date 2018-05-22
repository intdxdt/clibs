#include <iostream>
#include <memory>
#include <utility>
#include "mbr.h"

using namespace std;

int main() {
    cout << feq(0.1 + 0.2, 03) << endl;
    auto a = mbr::MBR{3, 4, 5, 6};
    auto b = mbr::MBR{1.5, 1.6, 1.1, 1.2};
    auto c = a + b;
    cout << c.minx << ", " << c.miny << endl;
    cout << c.maxx << ", " << c.maxy << endl;
    return 0;
}