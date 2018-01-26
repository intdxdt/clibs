//
// Created by titus on 25/01/18.
//
#include <stdio.h>
#include "mem.h"

#undef malloc

/**
 * ask and check memory requested
 */
void *alloc(size_t size) {
    void *new_mem = malloc(size);
    if (new_mem == NULL) {
        printf("out of memory !\n");
        exit(EXIT_FAILURE);
    }
    return new_mem;
}
