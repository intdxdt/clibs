//
// Created on 11/08/18.
//
#include <functional>
#include "util.h"
#include "brute.h"
#include "intersects.h"

#ifndef RBSEGS_SEG_H
#define RBSEGS_SEG_H


bool add_segment(
        size_t index,
        const std::vector<Seg>& red,
        BruteForceList& redList,
        const std::vector<Seg>& blue,
        BruteForceList& blueList,
        const std::function<bool(size_t, size_t)>& visit,
        bool flip
) {

    const Seg& seg = red[index];

    //Read out components
    double l0 = min(seg.a.y, seg.b.y);
    double h0 = max(seg.a.y, seg.b.y);

    //Scan over blue intervals for point
    auto count = blueList.count;
    auto ptr = 2 * count;
    double h1, l1;
    size_t bindex = 0;
    bool ret = false;


    for (auto i = count; !ret && (i-- > 0);) {
        ptr--;
        h1 = blueList.intervals[ptr];
        ptr--;
        l1 = blueList.intervals[ptr];

        //Test if intervals overlap
        if (l0 <= h1 && l1 <= h0) {
            bindex = blueList.index[i];
            const Seg& segb = blue[bindex];
            if (intersects(seg.a, seg.b, segb.a, segb.b)) {
                ret = flip ? visit(bindex, index) : visit(index, bindex);
            }
        }
    }

    redList.insert(l0, h0, index);
    return false;
}

#endif //RBSEGS_SEG_H
