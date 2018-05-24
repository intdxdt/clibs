#include "../mutil/mutil.h"
#include <cmath>

struct BoxObj {
    mbr::MBR& BBox(){

    }
};

 int maxEntries(int x) const {
    return std::min(4, x);
}

 int minEntries(int x) const {
    return std::max(2, int(math::ceil(float64(x) * 0.4)));
}

//compareNodeMinX computes change in minimum x
float64 compareNodeMinX(const BoxObj& a, const BoxObj& b) const {
    return (*a).BBox()[x1] - (*b).BBox()[x1];
}

//compareNodeMinY computes change in minimum y
 float64 compareNodeMinY(const BoxObj& a, const BoxObj& b) const {
    return a.BBox()[y1] - b.BBox()[y1];
}

swapItem(arr []BoxObj, i, j int) {
    arr[i], arr[j] = arr[j], arr[i];
}

 (int, []int) popInt([]int a) const {
    auto n, v int;
    n = len(a) - 1;
    v, a[n] = a[n], 0;
    a = a[:n];
    return v, a;
}


 []int appendInts([]int a, ::::::int v) const {
    auto for (i = range v ) {
        a = append(a, v[i]);
    }
    return a;
}
