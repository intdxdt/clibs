//
// Created on 11/08/18.
//
#include <vector>
#include "util.h"
#include "intersects.h"

#ifndef RBSEGS_BRUTE_FORCE_H
#define RBSEGS_BRUTE_FORCE_H

std::vector<std::vector<int>> BruteForce(Segs red, Segs blue) {
    auto nr = red.size();
    auto nb = blue.size();
    std::vector<std::vector<int>> crossings;
    Seg* rseg, * bseg;

    for (auto i = 0; i < nr; i++) {
        rseg = &red[i];
        for (auto j = 0; j < nb; j++) {
            bseg = &blue[j];
            if (intersects(&rseg[0], &rseg[1], &bseg[0], &bseg[1])) {
                crossings.emplace_back(std::vector<int>{i, j});
            }
        }
    }
    return crossings;
}

#endif //RBSEGS_BRUTE_FORCE_H
