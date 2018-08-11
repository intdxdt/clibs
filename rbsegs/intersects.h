//
// Created on 11/08/18.
//
#include <vector>
#include <array>
#include "util.h"
#include "../mbr/mbr.h"
#include "../mutil/mutil.h"

#ifndef RBSEGS_INTERSECTS_H
#define RBSEGS_INTERSECTS_H

//envelope of segment
inline MBR bbox(const std::vector<double>& a, const std::vector<double>& b) {
    return MBR(a[x], a[y], b[x], b[y]);
}

//clamp to zero if float is near zero
inline double snap_to_zero(double v) {
    return feq(v, 0.0) ? 0.0 : v;
}

//clamp to zero or one
inline double snap_to_zero_or_one(double v) {
    return feq(v, 0.0) ? 0.0 : feq(v, 1.0) ? 1.0 : v;
}


//do two lines intersect line segments a && b with
//vertices sa, sb, oa, ob
bool intersects(const Pt& sa, const Pt& sb, const Pt& oa, const Pt& ob) {
    auto bln = false;
    auto a = ((ob[0] - oa[0]) * (sa[1] - oa[1])) - ((ob[1] - oa[1]) * (sa[0] - oa[0]));
    auto b = ((sb[0] - sa[0]) * (sa[1] - oa[1])) - ((sb[1] - sa[1]) * (sa[0] - oa[0]));
    auto d = ((ob[1] - oa[1]) * (sb[0] - sa[0])) - ((ob[0] - oa[0]) * (sb[1] - sa[1]));

    //snap to zero if near -0 or 0
    a = snap_to_zero(a);
    b = snap_to_zero(b);
    d = snap_to_zero(d);

    if (d == 0.0) {
        if (a == 0.0 && b == 0.0) {
            bln = bbox(sa, sb).intersects(bbox(oa, ob));
        }
        return bln;
    }
    //intersection along the the seg or extended seg
    auto ua = snap_to_zero_or_one(a / d);
    auto ub = snap_to_zero_or_one(b / d);
    return (0 <= ua && ua <= 1) && (0 <= ub && ub <= 1);
}

#endif //RBSEGS_INTERSECTS_H
