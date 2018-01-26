//
// Created by titus on 25/01/18.
//

#ifndef MEM_MEM_H
#define MEM_MEM_H
/*
 * definitions for a less error-prone memory allocator
 */
#include <stdlib.h>

#define  malloc DONT'T CALL malloc DIRECTLY !
#define MALLOC(num, type) (type *) alloc( (num) * sizeof(type))

extern void *alloc(size_t size);

#endif //MEM_MEM_H
