// Adapted from BurntSushi/clibs

#ifndef VECTOR_VECTOR_H
#define VECTOR_VECTOR_H

#include <assert.h>
#include <stdint.h>

typedef  int32_t i32;

/* some private constants for vector tuning */
static const i32 VECTOR_BASE_CAPACITY = 10;
static const float VECTOR_EXPAND_RATIO = 1.5;


typedef struct {
    i32 size;
    i32 capacity;
    void **data;
} Vector;

/**
 * Creates a vector with DS_VECTOR_BASE_CAPACITY.
 * vector_free or vector_free_no_data will need to be called
 * when done with the vector to avoid memory leaks.
 */
Vector *
vector_create();

/**
 * Creates a vector with the given capacity.
 * (N.B. This vector will still automatically increase in size if necessary.)
 */
Vector *
vector_create_capacity(i32 capacity);

/**
 * Free's a vector AND its data.
 */
void
vector_free(Vector *vec);

/**
 * Free's just the vector's representation. Data is NOT freed.
 */
void
vector_free_no_data(Vector *vec);

/**
 * Copies a vector, but not the data.
 * Make sure you free it!
 */
Vector *
vector_copy(Vector *vec);

/**
 * Adds an element to the end of a vector.
 * Runs in constant time.
 */
void
vector_append(Vector *vec, void *data);

/**
 * Places an element at index i, and shifts the rest of the vector
 * to the right by one. If index == size of vector, then the element
 * will be appended to the end of the vector.
 */
void
vector_insert(Vector *vec, void *data, i32 index);

/**
 * Removes an element from the vector at some index.
 * Also shifts everything to right of index to the left.
 * Does *NOT* free the data.
 */
void
vector_remove(Vector *vec, i32 index);

/**
 * Gets an element at index i from a vector.
 */
void *
vector_get(Vector *vec, i32 index);

/**
 * Sets an elements at index to data.
 * No data is freed.
 */
void
vector_set(Vector *vec, void *data, i32 index);

/**
 * Swaps the data from vector element i to vector element j.
 */
void
vector_swap(Vector *vec, i32 i, i32 j);

/**
 * Higher-order function to map func over every element in vector.
 */
void
vector_map(Vector *vec, void (func)(void *));

/**
 * Higher-order function to find the first element in the vector
 * such that compare(needle, element) == 0.
 * Where compare is defined as:
 *      compare(a, b) < 0 when a < b
 *      compare(a, b) = 0 when a = b
 *      compare(a, b) > 0 when a > b
 *
 * Returns an unsigned -1 if needle is not found.
 */
i32
vector_find(Vector *vec, void *needle, i32 (compare)(void *, void *));

/**
 * Runs quicksort on the vector in place.
 */
void
vector_sort(Vector *vec, i32 (compare)(void *, void *));

#endif //VECTOR_VECTOR_H
