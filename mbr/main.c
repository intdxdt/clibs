#include <stdio.h>
#include <assert.h>
#include "mbr.h"

int main() {
    mbr box = new_mbr(2.4, 2.8, 1, 1.6);
    mbr box_clone = new_mbr(2.4, 2.8, 1, 1.6);
    assert(box.minx == 1);
    assert(box.miny == 1.6);
    assert(box.maxx == 2.4);
    assert(box.maxy == 2.8);
    assert(mbr_equals(&box, &box_clone));
    printf("%s", "All Tests Passed !");
    return 0;
}