//
// Created on 11/08/18.
//

#include <vector>
#include <algorithm>
#include "Event.h"
#include "util.h"


#ifndef RBSEGS_PREP_H
#define RBSEGS_PREP_H

void prepareEvents(const std::vector<Seg>& red, const std::vector<Seg>& blue, std::vector<Event>& data) {
    size_t i = 0;
    for (auto& seg : red) {
        data.emplace_back(Event{min(seg.a.x, seg.b.x), CreateRED, i});
        data.emplace_back(Event{max(seg.a.x, seg.b.x), RemoveRED, i});
        i++;
    }

    i = 0;
    for (auto& seg : blue) {
        data.emplace_back(Event{min(seg.a.x, seg.b.x), CreateBLUE, i});
        data.emplace_back(Event{max(seg.a.x, seg.b.x), RemoveBLUE, i});
        i++;
    }

    std::sort(data.begin(), data.end(), lex_events());
}


#endif //RBSEGS_PREP_H
