//
// Created by titus on 03/01/18.
//

#include "mbr.h"
#include "test.h"

int main() {
    mbr box = new_mbr(2.4, 2.8, 1, 1.6);
    mbr box_clone = new_mbr(2.4, 2.8, 1, 1.6);
    check(box.minx == 1.0);
    check(box.miny == 1.6);
    check(box.maxx == 2.4);
    check(box.maxy == 2.8);
    check(mbr_equals(&box, &box_clone));
    char buf[1000];
    check(mbr_wkt(&box, buf) > 0);
    printf("%s\n", buf);
    return 0;
}