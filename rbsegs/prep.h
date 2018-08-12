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
    std::vector<event> data(2 * n);
    size_t ptr = 0;

    for (size_t i = 0; i < nr; i++) {
        seg = red[i];
        x = seg[0][0];
        y = seg[1][0];
        data[ptr].val = min(x, y);
        data[ptr].ev = Ev::CreateRED;
        data[ptr].idx = static_cast<int>(i);
        ptr++;

        data[ptr].val = max(x, y);
        data[ptr].ev = Ev::RemoveRED;
        data[ptr].idx = static_cast<int>(i);
        ptr++;
    }

    for (size_t i = 0; i < nb; i++) {
        seg = blue[i];
        x = seg[0][0];
        y = seg[1][0];

        data[ptr].val = min(x, y);
        data[ptr].ev = Ev::CreateBLUE;
        data[ptr].idx = static_cast<int>(i);
        ptr++;

        data[ptr].val = max(x, y);
        data[ptr].ev = Ev::RemoveBLUE;
        data[ptr].idx = static_cast<int>(i);
        ptr++;
    }
    std::sort(data.begin(), data.end(), lex_events());

    return data;
}

#endif //RBSEGS_PREP_H
