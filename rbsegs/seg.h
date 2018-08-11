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
         const Segs& red,  BruteForceList& redList,
         const Segs& blue, BruteForceList& blueList,
         const std::function<bool(int, int)>& visit, bool flip) {
    //Look up segment
    Seg seg = red[index];

    //Get segment end points
    Pt seg_a = seg[0];
    Pt seg_b = seg[1];

    //Read out components
    double  a0 = seg_a[1];
    double  a1 = seg_b[1];
    double l0 = min(a0, a1);
    double h0 = max(a0, a1);

    //Scan over blue intervals for point
    auto count = blueList.count;
    auto ptr = 2 * count;
    double h1, l1;
    int bindex = 0;
    bool ret = false;


    for (auto i = count - 1; !ret &&  i >= 0; i--) {
        ptr += -1;
        h1 = blueList.intervals[ptr];
        ptr += -1;
        l1 = blueList.intervals[ptr];

        //Test if intervals overlap
        if (l0 <= h1 && l1 <= h0) {
            bindex = blueList.index[i];
            //Test if segments intersect
            if (intersects(seg_a, seg_b, blue[bindex][0], blue[bindex][1])) {
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
