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
        const std::function<bool(size_t, size_t)>& visit) {
    auto nr = red.size();
    auto nb = blue.size();
    auto n = nr + nb;
    auto ne = 2 * n;
    auto ret = false;
    std::vector<Event> events;
    events.reserve(ne);

    prepareEvents(red, blue, events);
    BruteForceList redList{nr};
    BruteForceList blueList{nb};
    size_t ev ;

    for (size_t i = 0; !ret && i < ne; ++i) {
        ev = events[i].ev;
        if (ev == CreateRED) {
            ret = addSegment(events[i].idx, red, redList, blue, blueList, visit, false);
        } else if (ev == CreateBLUE) {
            ret = addSegment(events[i].idx, blue, blueList, red, redList, visit, true);
        } else if (ev == RemoveRED) {
            redList.remove(events[i].idx);
        } else if (ev == RemoveBLUE) {
            blueList.remove(events[i].idx);
        }
    }

    return ret;
}

std::vector<std::vector<size_t>> RBIntersection(const Segs& red, const Segs& blue) {
    std::vector<std::vector<size_t>> crossings;
    auto visit = [&](size_t i, size_t j) {
        crossings.emplace_back(std::vector<size_t>{i, j});
        return false;
    };
    RedBlueLineSegmentIntersection(red, blue, visit);
    return crossings;
}

#endif //RBSEGS_RBINTER_H
