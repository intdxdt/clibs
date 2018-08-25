
#include "../mutil/mutil.h"

#ifndef RBSEGS_EVENT_H
#define RBSEGS_EVENT_H

enum  Ev{CreateRED, CreateBLUE, RemoveRED, RemoveBLUE};

struct Event {
    double val;
    Ev ev;
    size_t idx;
};

struct lex_events {
    inline bool operator()(const Event& a, const Event& b) {
        auto d = a.val - b.val;
        if (feq(d, 0)){
            return (a.ev != b.ev ? a.ev < b.ev : a.idx < b.idx);
        }
        return d < 0;
    }
};
#endif //RBSEGS_EVENT_H
