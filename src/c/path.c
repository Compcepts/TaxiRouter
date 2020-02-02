/*
Implementation of paths


METHODS:


*/

#include <stdlib.h>

#include "../defs/types.h"
#include "../defs/const.h"

#include "../h/path.h"


path* new_path(edge *e) {
    path* pth;

    pth = (path*) malloc(sizeof(path));

    pth->curr_road = e;
    pth->next_path = NULL;

    return pth;
}

path* empty_path() {
    path* pth;

    pth = (path*) malloc(sizeof(path));

    pth->curr_road = NULL;
    pth->next_path = NULL;

    return pth;
}

void append_path(path *begin, path *end) {
    begin->next_path = end;
}

path* copy_path(path *p) {
    path *copy = empty_path();

    copy->curr_road = p->curr_road;
    copy->next_path = p->next_path;

    return copy;
}

path* pop_path_head(path **head){
    path *next = (*head)->next_path;
    delete_path(head);
    return next;
}

void delete_path(path **old) {
    (*old)->curr_road = NULL;
    (*old)->next_path = NULL;

    free(*old);
}


void delete_full_path(path **head) {
    while((*head)->next_path != NULL) {
        delete_full_path(&(*head)->next_path);
    }
    delete_path(head);
}