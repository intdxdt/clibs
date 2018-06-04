#include <iostream>
#include "rand.h"

int main() {
    auto r = URandom(0, 1);
    std::cout << r.random() << std::endl;
    std::cout << r.random() << std::endl;
    std::cout << r.random() << std::endl;
    return 0;
}