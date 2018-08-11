//
// Created on 11/08/18.
//

#include <vector>
#include <algorithm>
#include "event.h"
#include "util.h"


#ifndef RBSEGS_PREP_H
#define RBSEGS_PREP_H

std::vector<event> prepareEvents(const Segs& red, const Segs& blue) {
    auto nr = red.size();
    auto nb = blue.size();
    auto n = nr + nb;
    double x, y;
    Seg seg;
    std::vector<event> data;
    data.reserve(2 * n);

    for (auto i = 0; i < nr; i++) {
        seg = red[i];
        x = seg[0][0];
        y = seg[1][0];
        data.emplace_back(event{min(x, y), Ev::CreateRED, i});
        data.emplace_back(event{max(x, y), Ev::RemoveRED, i});
    }

    for (auto i = 0; i < nb; i++) {
        seg = blue[i];
        x = seg[0][0];
        y = seg[1][0];
        data.emplace_back(event{min(x, y), Ev::CreateBLUE, i});
        data.emplace_back(event{max(x, y), Ev::RemoveBLUE, i});
    }
    std::sort(data.begin(), data.end(), lex_events());
    return data;
}

#endif //RBSEGS_PREP_H
