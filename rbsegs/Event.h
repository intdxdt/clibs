
#include "../mutil/mutil.h"

#ifndef RBSEGS_EVENT_H
#define RBSEGS_EVENT_H


const size_t CreateRED  = 0;
const size_t CreateBLUE = 1;
const size_t RemoveRED  = 2;
const size_t RemoveBLUE = 3;

struct Event {
    double val;
    size_t ev;
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
