#include <stdio.h>
#include "sllist.h"

int main() {
    Node *root = NULL;
    ssl_insert(&root, 3);
    ssl_insert(&root, 2);
    ssl_insert(&root, 5);
    ssl_insert(&root, 1);
    ssl_insert(&root, 0);
    return 0;
}