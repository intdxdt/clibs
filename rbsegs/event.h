
#include "../mutil/mutil.h"

#ifndef RBSEGS_EVENT_H
#define RBSEGS_EVENT_H

enum Ev {
    CreateRED, CreateBLUE, RemoveRED, RemoveBLUE
};

struct event {
    double val;
    int ev;
    int idx;
};

struct events {
    template<typename T>
    inline bool operator()(const T& a, const T& b) {
        auto d = a.val - b.val;
        int
        id
        //x's are close enough to each other
        if (feq(d, 0)) {
            id = a.ev - b.ev
        } else {
            return d < 0
        }

        //y's are close enough to each other
        if (id == 0) {
            id = a.idx - b.idx
        }
        return id < 0
    }
};

#endif //RBSEGS_EVENT_H
