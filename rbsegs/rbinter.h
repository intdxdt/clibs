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
        const std::vector<Seg>& red, const std::vector<Seg>& blue,
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

    for (size_t i = 0; !ret && i < ne; ++i) {
        switch (events[i].ev) {
            case CreateRED:
                ret = add_segment(events[i].idx, red, redList, blue, blueList, visit, false);
                break;
            case CreateBLUE:
                ret = add_segment(events[i].idx, blue, blueList, red, redList, visit, true);
                break;
            case RemoveRED:
                redList.remove(events[i].idx);
                break;
            case RemoveBLUE :
                blueList.remove(events[i].idx);
                break;
        }
    }

    return ret;
}

std::vector<std::vector<size_t>> rb_intersection(const std::vector<Seg>& red, const std::vector<Seg>& blue) {
    std::vector<std::vector<size_t>> crossings;
    RedBlueLineSegmentIntersection(red, blue, [&](size_t i, size_t j) {
        crossings.emplace_back(std::vector<size_t>{i, j});
        return false;
    });
    return crossings;
}

#endif //RBSEGS_RBINTER_H
