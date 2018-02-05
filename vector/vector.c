//

#include <stdlib.h>
#include <string.h>

#include "vector.h"

/* private function to check and possibly expand a vector's capacity */
static void
vector_maybe_expand(Vector *vec);

/* private helper functions for quicksort */
static void
vector_quicksort(Vector *vec, int left, int right, int (compare)(void *, void *));

static int
vector_partition(Vector *vec, int left, int right, int pivot, int (compare)(void *, void *));


Vector *
vector_create() {
    return vector_create_capacity(VECTOR_BASE_CAPACITY);
}


Vector *
vector_create_capacity(i32 capacity) {
    Vector *vec;

    vec = malloc(sizeof(*vec));
    assert(vec);

    vec->size = 0;
    vec->capacity = capacity;
    vec->data = malloc(vec->capacity * sizeof(*vec->data));
    assert(vec->data);

    return vec;
}

void
vector_free(Vector *vec) {
    i32 i;
    for (i = 0; i < vec->size; ++i)
        free(vec->data[i]);

    free(vec->data);
    free(vec);
}

void
vector_free_not_data(Vector *vec) {
    free(vec->data);
    free(vec);
}

Vector *
vector_copy(Vector *vec) {
    Vector *copy;
    i32 i;
    copy = vector_create_capacity(vec->capacity);
    for (i = 0; i < vec->size; ++i)
        vector_append(copy, vec->data[i]);
    return copy;
}

void
vector_append(Vector *vec, void *data) {
    vector_maybe_expand(vec);
    vec->data[vec->size] = data;
    ++vec->size;
}

void
vector_insert(Vector *vec, void *data, i32 index) {
    i32 i;
    //the index is allowed to be vec->size
    if (index > vec->size)
        return;
    vector_maybe_expand(vec);
    for (i = vec->size - 1; i >= index && i < (i + 1); --i)
        vec->data[i + 1] = vec->data[i];

    vec->data[index] = data;
    ++vec->size;
}

void
vector_remove(Vector *vec, i32 index) {
    i32 i;
    if (index >= vec->size)
        return;
    --vec->size;
    for (i = index; i < vec->size; ++i)
        vec->data[i] = vec->data[i + 1];
}

void *
vector_get(Vector *vec, i32 index) {
    if (index >= vec->size)
        return NULL;
    return vec->data[index];
}

//set at index
void
vector_set(Vector *vec, void *data, i32 index) {
    if (index >= vec->size)
        return;
    vec->data[index] = data;
}


//swap
void
vector_swap(Vector *vec, i32 i, i32 j) {
    void *temp;
    if (i >= vec->size || j >= vec->size)
        return;
    temp = vec->data[i];
    vec->data[i] = vec->data[j];
    vec->data[j] = temp;
}

//vector map
void
vector_map(Vector *vec, void(func)(void *)) {
    for (i32 i = 0; i < vec->size; i++)
        func(vec->data[i]);
}

i32
vector_find(Vector *vec, void *needle, i32(compare)(void *, void *)) {
    for (i32 i = 0; i < vec->size; ++i) {
        if (compare(needle, vec->data[i]) == 0)
            return i;
    }
    return -1;
}


static void
vector_maybe_expand(Vector *vec) {
    if (vec->size < vec->capacity)
        return;

    vec->capacity *= VECTOR_EXPAND_RATIO;
    vec->data = realloc(vec->data, vec->capacity * sizeof(vec->data));
    assert(vec->data);
}


/**
 * I've implemented my own Quicksort (based on the Wikipedia entry on
 * Quicksort) because I didn't like how the standard library's version
 * of qsort forces you to define the comparison function. (Namely, that
 * the arguments are void** instead of void*.)
 */
void
vector_sort(Vector *vec, i32 (compare)(void *, void *)) {
    vector_quicksort(vec, 0, vec->size - 1, compare);
}

static void
vector_quicksort(Vector *vec, i32 left, i32 right, i32 (compare)(void *, void *)) {
    i32 pivot;
    if (left >= right)
        return;
    pivot = (left + right) / 2;
    pivot = vector_partition(vec, left, right, pivot, compare);

    vector_quicksort(vec, left, pivot - 1, compare);
    vector_quicksort(vec, pivot + 1, right, compare);
}

static i32
vector_partition(Vector *vec, i32 left, i32 right, i32 pivot, i32 (compare)(void *, void *)) {
    void *pivot_val, *temp;
    i32 store_ind, i;

    pivot_val = vec->data[pivot];
    vec->data[pivot] = vec->data[right];
    vec->data[right] = pivot_val;

    store_ind = left;

    for (i = left; i < right; ++i)
        if (compare(vec->data[i], pivot_val) < 0) {
            temp = vec->data[i];
            vec->data[i] = vec->data[store_ind];
            vec->data[store_ind] = temp;
            ++store_ind;
        }

    temp = vec->data[store_ind];
    vec->data[store_ind] = vec->data[right];
    vec->data[right] = temp;

    return store_ind;
}
