
#include "../mutil/mutil.h"

#ifndef RBSEGS_EVENT_H
#define RBSEGS_EVENT_H

enum Ev {
    CreateRED, CreateBLUE, RemoveRED, RemoveBLUE
};

struct event {
    double val;
    Ev ev;
    int idx;
};

struct lex_events {
    template<typename T>
    bool operator()(const T& a, const T& b) {
        auto d = a.val - b.val;
        int id;
        if (feq(d, 0)) {
            id = a.ev - b.ev;
        } else {
            return d < 0;
        }

        if (id == 0) {
            id = a.idx - b.idx;
        }
        return id < 0;
    }
};

#endif //RBSEGS_EVENT_H
