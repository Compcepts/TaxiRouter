/*
Implementation of paths


METHODS:


*/


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

void append_path(path *begin, path *end) {
    begin->next_path = end;
}

int path_weight(path *start, int i) {
    int sum = 0;
    while(start->next_path != NULL) {
        sum += weight(start->curr_road, i);
        start = start->next_path;
    }
    sum += weight(start->curr_road, i);
    return sum;
}

path* pop_path_head(path **head){
    path *next = (*head)->next_path;
    delete_path(head);
    return next;
}

void delete_path(path **old) {
    (*old)->curr_road = NULL;
    (*old)->next_path = NULL;

    free(**old);
    (*old) = NULL;
}


void delete_full_path(path **head) {
    while((*head)->next_path != NULL) {
        delete_full_path(&(*head)->next_path);
    }
    delete_path(head);
}