#include <vector>
#include "util.h"
#include "intersects.h"

#ifndef RBSEGS_BRUTE_FORCE_H
#define RBSEGS_BRUTE_FORCE_H

std::vector<std::vector<size_t>> brute_force(const std::vector<Seg>& red, const std::vector<Seg>& blue) {
    auto nr = red.size();
    auto nb = blue.size();
    std::vector<std::vector<size_t>> crossings;
    for (size_t i = 0; i < nr; i++) {
        auto& pt = red[i];
        for (size_t j = 0; j < nb; j++) {
            if (intersects(pt.a, pt.b, blue[j].a, blue[j].b)) {
                crossings.emplace_back(std::vector<size_t>{i, j});
            }
        }
    }
    return crossings;
}

#endif //RBSEGS_BRUTE_FORCE_H
