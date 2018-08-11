//
// Created on 11/08/18.
//
#include <functional>
#include "util.h"
#include "brute.h"
#include "intersects.h"

#ifndef RBSEGS_SEG_H
#define RBSEGS_SEG_H


bool addSegment(
        int index,
        const Segs& red, BruteForceList& redList,
        const Segs& blue, BruteForceList& blueList,
        const std::function<bool(int, int)>& visit, bool flip) {
    //Look up segment
    const Seg* seg = &red[index];
    const Seg* bseg;

    //Get segment end points
    const Seg* x0 = &seg[0];
    const Seg* x1 = &seg[1];

    //Read out components
    double  a0 = x0[1];
    double  a1 = x1[1];
    auto l0 = min(a0, a1);
    auto h0 = max(a0, a1);

    //Scan over blue intervals for point
    auto intervals = blueList.intervals;
    auto blueIndex = blueList.index;
    auto count = blueList.count;
    auto ptr = 2 * count;
    double h1, l1;
    int bindex = 0;
    bool ret = false;


    for (auto i = count - 1; !ret &&  i >= 0; i--) {
        ptr += -1;
        h1 = intervals[ptr];
        ptr += -1;
        l1 = intervals[ptr];

        //Test if intervals overlap
        if (l0 <= h1 && l1 <= h0) {
            bindex = blueIndex[i];
            bseg   = &blue[bindex];

            //Test if segments intersect
            if (intersects(&seg[0], &seg[1], &bseg[0], &bseg[1])) {
                if (flip) {
                    ret = visit(bindex, index);
                } else {
                    ret = visit(index, bindex);
                }
            }
        }
    }

    redList.insert(l0, h0, index);
    return false;
}

#endif //RBSEGS_SEG_H
