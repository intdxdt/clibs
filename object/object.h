// Created on 24/05/18.
#include <array>
#include <cmath>

#ifndef OBJECT_STRUCT_H
#define OBJECT_STRUCT_H

using Bounds = std::array<double, 4>;

struct Object {
    virtual Bounds bounds() {
        return Bounds{
                std::nan("0"), std::nan("0"), std::nan("0"), std::nan("0")
        };
    }
};
#endif //OBJECT_STRUCT_H
