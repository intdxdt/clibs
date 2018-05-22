#include <iostream>
#include <memory>
#include <utility>
#include "mbr.h"

using namespace std;

int main() {
    cout << feq(0.1 + 0.2, 03) << endl;
    auto a = mbr::MBR{3, 4, 5, 6};
    auto b = mbr::MBR{1.5, 1.5, 4.1, 4.5};
    auto c = (a & b).value();
    auto d = (a | b);
    cout << a.wkt() << endl;
    cout << b.wkt() << endl;
    cout << c.wkt() << endl;
    cout << d.wkt() << endl;
    return 0;
}