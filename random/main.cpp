#include <iostream>
#include "rand.h"

void simple_printf(const char* fmt...) {
    va_list args;
    va_start(args, fmt);
    while (*fmt != '\0') {
        if (*fmt == 'd') {
            int i = va_arg(args, int);
            std::cout << i << '\n';
        } else if (*fmt == 'c') {
            // note automatic conversion to integral type
            int c = va_arg(args, int);
            std::cout << static_cast<char>(c) << '\n';
        } else if (*fmt == 'f') {
            double d = va_arg(args, double);
            std::cout << d << '\n';
        }
        ++fmt;
    }

    va_end(args);
    std::cout << args << std::endl;
}

int main() {
    auto r = URandom(0, 1);
    std::cout << r.random() << std::endl;
    std::cout << r.random() << std::endl;
    std::cout << r.random() << std::endl;
    simple_printf("dff", 2, 5.8, 6.68);
    return 0;
}