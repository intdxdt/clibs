#include <vector>
#include <algorithm>
#include "../pt/pt.h"
#include "../mbr/mbr.h"
#include "../mutil/mutil.h"

#ifndef RBSEGS_UTIL_H
#define RBSEGS_UTIL_H
namespace rbsegs {
    enum Ev {
        CreateRED, CreateBLUE, RemoveRED, RemoveBLUE
    };
    struct Seg {
        Pt a;
        Pt b;
    };

    struct Event {
        double val;
        Ev ev;
        size_t idx;
    };

    template<typename T>
    inline T min(T a, T b) {
        return b < a ? b : a;
    };

    template<typename T>
    inline T max(T a, T b) {
        return b > a ? b : a;
    };

    struct lex_events {
        inline bool operator()(const Event& a, const Event& b) {
            auto d = a.val - b.val;
            if (feq(d, 0)) {
                return (a.ev != b.ev ? a.ev < b.ev : a.idx < b.idx);
            }
            return d < 0;
        }
    };

    void prepare_events(const std::vector<Seg>& red, const std::vector<Seg>& blue, std::vector<Event>& data) {
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
}

#endif //RBSEGS_UTIL_H
