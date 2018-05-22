#include <iostream>
#include "mutil.h"

int main() {
    std::cout << (0.1 + 0.2 == 0.3) << std::endl;
    std::cout << feq(0.1 + 0.2, 0.3, 1e-12) << std::endl;
    std::cout << "Hello, World!" << std::endl;
    return 0;
}