//
// Created on 11/08/18.
//
#include <functional>
#include "util.h"
#include "brute.h"


#ifndef RBSEGS_RBINTER_H
#define RBSEGS_RBINTER_H
namespace rbsegs {
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

    bool redblue_segment_intersection(
            const std::vector<Seg>& red, const std::vector<Seg>& blue,
            const std::function<bool(size_t, size_t)>& visit) {
        auto nr = red.size();
        auto nb = blue.size();
        auto n = nr + nb;
        auto ne = 2 * n;
        auto ret = false;
        std::vector<Event> events;
        events.reserve(ne);

        prepare_events(red, blue, events);
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
        redblue_segment_intersection(red, blue, [&](size_t i, size_t j) {
            crossings.emplace_back(std::vector<size_t>{i, j});
            return false;
        });
        return crossings;
    }
}
#endif //RBSEGS_RBINTER_H
