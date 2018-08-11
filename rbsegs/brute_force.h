//
// Created on 11/08/18.
//
#include <vector>
#include "util.h"
#include "intersects.h"

#ifndef RBSEGS_BRUTE_FORCE_H
#define RBSEGS_BRUTE_FORCE_H

std::vector<std::vector<int>> BruteForce(const Segs& red, const Segs& blue) {
    auto nr = red.size();
    auto nb = blue.size();
    std::vector<std::vector<int>> crossings;
    for (size_t i = 0; i < nr; i++) {
        for (size_t j = 0; j < nb; j++) {
            if (intersects(red[i][0], red[i][1], blue[j][0], blue[j][1])) {
                crossings.emplace_back(std::vector<int>{int(i), int(j)});
            }
        }
    }
    return crossings;
}

#endif //RBSEGS_BRUTE_FORCE_H
