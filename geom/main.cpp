#include <iostream>
#include <string>
#include "geom.h"

int main() {
    std::string ln{"LINESTRING (30 10, 10 30, 40 40)"};
    auto m = geom::wkt_type_coords(ln);
    return 0;
}