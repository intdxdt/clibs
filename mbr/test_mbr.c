//
// Created by titus on 03/01/18.
//

#include <string.h>
#include <stdio.h>
#include "mbr.h"
#include "test.h"

void test_mbr();

int main() {
    mbr box = new_mbr(2.4, 2.8, 1, 1.6);
    mbr box_clone = new_mbr(2.4, 2.8, 1, 1.6);
    check(box.minx == 1.0 && box.miny == 1.6);
    check(box.maxx == 2.4 && box.maxy == 2.8);
    check(mbr_equals(&box, &box_clone));

    char buf[1000];
    check(mbr_wkt(&box, buf) > 0);
    check(strlen(buf) > 10);

    test_mbr();
    return 0;
}

void test_mbr() {
    printf("\n%s\n\n", "------------test mbr --------------");
    mbr m0 = new_mbr(0.0, 0.0, 0.5, 0.2);
    mbr m1 = new_mbr(2.0, 2.0, -0.5, -0.2);
    mbr m = mbr_union(&m0, &m1);
    check(m.minx == -0.5);
    check(m.miny == -0.2);
    check(m.maxx == 2.0);
    check(m.maxy == 2.0);

    mbr mr = new_mbr_raw(2.0, 2.0, -0.5, -0.2);
    check(mr.minx == 2.0);
    check(mr.miny == 2.0);
    check(mr.maxx == -0.5);
    check(mr.maxy == -0.2);

    mbr m3 = new_mbr(2.0, 2.0, 0.5, 0.2);
    check(m3.minx == 0.5);
    check(m3.miny == 0.2);
    check(m3.maxx == 2.0);
    check(m3.maxy == 2.0);
    check(mbr_height(&m3) == 1.8);
    check(mbr_width(&m3) == 1.5);
    check(mbr_area(&m3) == 1.5 * 1.8);
    check(!mbr_is_point(&m3));
    double arr[4];
    mbr_as_array(&m3, arr);
    check(
        arr[0] == 0.5 &&
        arr[1] == 0.2 &&
        arr[2] == 2.0 &&
        arr[3] == 2.0
    );
}