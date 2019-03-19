#include <iostream>
#include "pt.h"

using namespace std;

int main() {
    Pt<int> a = {3, 4};
    Pt<int> a2 = {3, 4};

    Pt3d<int> b = {5, 6, 7};
    std::cout << (a == a2) << std::endl;
    std::cout << a.x << std::endl;
    std::cout << b.z << std::endl;
    std::cout << b.x << ", " << b.y << ", " << b.z << std::endl;
//    auto c = b;
//    auto d = a;
    return 0;
}