#include <stdio.h>
#include "mem.h"
#include "../mbr/test.h"

int main() {
    int *new_memory = MALLOC(25, int);
    for(int p = 0; p < 25; p++)
        new_memory[p] = p*2 ;
    check(new_memory[0] == 0);
    check(new_memory[3] == 6);
    check(new_memory[10] == 20);
    free(new_memory);
    return 0;
}