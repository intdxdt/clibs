#include <stdio.h>
#include "vector.h"

i32 cmp(void *a, void *b) {
    i32 *i = (i32 *) a;
    i32 *j = (i32 *) b;
    return (*i) - (*j);
}

int main() {
    Vector *v = vector_create_capacity(10);
    int a = 8;
    vector_append(v, &a);
    int b = 7;
    vector_append(v, &b);
    int c = 3;
    vector_append(v, &c);
    vector_sort(v, cmp);
    for (int i = 0; i < 3; i++) {
        int *o = (int *) v->data[i];
        printf("%d\n", *o);
    }
    return 0;
}