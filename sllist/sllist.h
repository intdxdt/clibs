//
// Created by resson on 27/01/18.
//

#ifndef SLLIST_SLL_H
#define SLLIST_SLL_H
#define  FALSE 0
#define  TRUE  1

#include <stdlib.h>

typedef struct NODE {
    struct NODE *link;
    int value;
} Node;

int ssl_insert(register Node **linkp, int new_value) {
    register Node *current;
    register Node *new;

    //walk down the list
    while ((current = *linkp) != NULL && current->value < new_value) {
        linkp = &current->link;
    }

    new = (Node *) malloc(sizeof(Node));
    if (new == NULL) {
        return FALSE;
    }
    new->value = new_value;

    //insert the new node into list, and return TRUE
    new->link = current;
    *linkp = new;

    return TRUE;
}

#endif //SLLIST_SLL_H
