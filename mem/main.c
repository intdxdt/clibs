#include <stdio.h>
#include "mem.h"

int main() {
    int *new_memory = MALLOC(25, int);
    for(int p = 0; p < 25; p++)
        new_memory[p] = p ;
    printf("%d\n", new_memory[3]);
    printf("%d\n", new_memory[10]);
    free(new_memory);
    return 0;
}