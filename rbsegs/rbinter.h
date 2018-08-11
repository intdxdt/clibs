//
// Created on 11/08/18.
//
#include <functional>
#include "util.h"
#include "prep.h"
#include "brute.h"
#include "seg.h"

#ifndef RBSEGS_RBINTER_H
#define RBSEGS_RBINTER_H

bool RedBlueLineSegmentIntersection(
        const Segs& red, const Segs& blue,
        const std::function<bool(int, int)>& visit) {
    auto nr = red.size();
    auto nb = blue.size();
    auto n = nr + nb;
    auto ne = 2 * n;
    auto ret = false;

    auto events = prepareEvents(red, blue);

    auto redList = BruteForceList(nr);
    auto blueList = BruteForceList(nb);

    for (auto i = 0; !ret && i < ne; i++) {
        auto ev = events[i].ev;
        auto index = events[i].idx;
        if (ev == Ev::CreateRED) {
            ret = addSegment(index, red,  redList, blue, blueList, visit, false);
        } else if (ev == Ev::CreateBLUE) {
            ret = addSegment(index, blue, blueList, red,  redList, visit, true);
        } else if (ev == Ev::RemoveRED) {
            redList.remove(index);
        } else if (ev == Ev::RemoveBLUE) {
            blueList.remove(index);
        }
    }

    return ret;
}

std::vector<std::vector<int>> RBIntersection(const Segs& red, const Segs& blue) {
    std::vector<std::vector<int>> crossings;
    RedBlueLineSegmentIntersection(red, blue, [&](int i, int j) {
        crossings.emplace_back(std::vector<int>{i, j});
        return false;
    });
    return crossings;
}

#endif //RBSEGS_RBINTER_H
