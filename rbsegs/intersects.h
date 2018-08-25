#include <vector>
#include <array>
#include "util.h"

#include "../mbr/mbr.h"
#include "../mutil/mutil.h"

#ifndef RBSEGS_INTERSECTS_H
#define RBSEGS_INTERSECTS_H

//envelope of segment
inline mbr::MBR bbox(const Pt& a, const Pt& b) {
    return mbr::MBR(a.x, a.y, b.x, b.y);
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
    auto a = ((ob.x - oa.x) * (sa.y - oa.y)) - ((ob.y - oa.y) * (sa.x - oa.x));
    auto b = ((sb.x - sa.x) * (sa.y - oa.y)) - ((sb.y - sa.y) * (sa.x - oa.x));
    auto d = ((ob.y - oa.y) * (sb.x - sa.x)) - ((ob.x - oa.x) * (sb.y - sa.y));

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
