/*
Implementation of paths


METHODS:


*/

#include <stdlib.h>
#include <stdio.h>

#include "../defs/types.h"
#include "../defs/const.h"

#include "../h/path.h"
#include "../h/graph.h"


path* new_path(edge *e) {
    path* pth;

    pth = (path*) malloc(sizeof(path));

    pth->edge_queue_tail = new_eq(e);
    pth->cost = 0;
    pth->length = 0;
    pth->start = NULL;

    return pth;
}

path* empty_path() {
    path* pth;

    pth = (path*) malloc(sizeof(path));

    pth->edge_queue_tail = NULL;
    pth->cost = 0;
    pth->length = 0;
    pth->start = NULL;

    return pth;
}

edge_queue* new_eq(edge *e) {
    edge_queue *eq;
    eq = (edge_queue*) malloc(sizeof(edge_queue));
    eq->curr_edge = e;
    eq->next_eq = NULL;

    return eq;
}

void enqueue_edge(edge_queue **tail, edge *e) {
    edge_queue *next;
    if (*tail == NULL) {
        *tail = new_eq(e);
        (*tail)->next_eq = *tail;
    }
    else if ((*tail)->next_eq == *tail) {
        next = new_eq(e);
        next->next_eq = *tail;
        (*tail)->next_eq = next;
        *tail = next;
    }
    else {
        next = new_eq(e);
        next->next_eq = (*tail)->next_eq;
        (*tail)->next_eq = next;
        *tail = next;
    }
}

edge* dequeue_edge(edge_queue **tail) {
    edge *e;
    edge_queue *remove_eq;

    if (*tail == NULL) {
        return NULL;
    }
    else if ((*tail)->next_eq == *tail) {
        e = (*tail)->curr_edge;
        (*tail)->next_eq = NULL;
        remove_eq = *tail;
        *tail = NULL;

        delete_eq(remove_eq);
    }
    else {
        e = (*tail)->next_eq->curr_edge;
        remove_eq = (*tail)->next_eq;
        (*tail)->next_eq = (*tail)->next_eq->next_eq; 

        delete_eq(remove_eq);
    }
    return e;
}

edge* queue_head_edge(path *p) {
    if(p->edge_queue_tail == NULL) {
        return NULL;
    }
    if(p->edge_queue_tail == p->edge_queue_tail->next_eq) {
        return p->edge_queue_tail->curr_edge;
    }
    return p->edge_queue_tail->next_eq->curr_edge;
}

path* copy_path(path *p) {
    path *copy = empty_path();
    edge_queue *tail = p->edge_queue_tail, *next_eq;

    copy->cost = p->cost;
    copy->length = p->length;
    copy->start = p->start;

    if (tail == NULL) {
        copy->edge_queue_tail = NULL;
    }
    else if (tail == tail->next_eq) {
        enqueue_edge(&(copy->edge_queue_tail), tail->curr_edge);
    }
    else {
        next_eq = tail->next_eq;
        while (next_eq != tail) {
            enqueue_edge(&(copy->edge_queue_tail), next_eq->curr_edge);
            next_eq = next_eq->next_eq;
        }
        enqueue_edge(&(copy->edge_queue_tail), tail->curr_edge);
    }
    return copy;
}

void decrement_edges(path *p, int c) {
    edge_queue *tail = p->edge_queue_tail, *next;
    
    if (tail == NULL) {
        return;
    }
    else if (tail == tail->next_eq) {
        tail->curr_edge->weight[c]--;
        opposite_edge(tail->curr_edge)->weight[c]--;
    }
    else {
        next = tail->next_eq;
        while(next != tail) {
            next->curr_edge->weight[c]--;
            opposite_edge(next->curr_edge)->weight[c]--;
            next = next->next_eq;
        }
        next->curr_edge->weight[c]--;
    }
}

void increment_edges(path *p, int c) {
    edge_queue *tail = p->edge_queue_tail, *next;
    
    if (tail == NULL) {
        return;
    }
    else if (tail == tail->next_eq) {
        tail->curr_edge->weight[c]++;
        opposite_edge(tail->curr_edge)->weight[c]++;
    }
    else {
        next = tail->next_eq;
        while(next != tail) {
            next->curr_edge->weight[c]++;
            opposite_edge(next->curr_edge)->weight[c]++;
            next = next->next_eq;
        }
        next->curr_edge->weight[c]++;
    }
}

void add_cost(path *p, int c) {
    p->cost += c;
}

void add_length(path *p, int l) {
    p->length += l;
}

vertex* next_vertex(path *p) {
    if (p == NULL) {
        return NULL;
    }
    if (p->edge_queue_tail == NULL) {
        return p->start;
    }
    return p->edge_queue_tail->curr_edge->dest;
}

bool empty_eq(edge_queue *tail) {
    if (tail == NULL) {
        return TRUE;
    }
    return FALSE;
}

void delete_eq(edge_queue *eq) {
    eq->curr_edge = NULL;
    eq->next_eq = NULL;

    free(eq);
}

void delete_full_eq(edge_queue **tail) {
    while (*tail != NULL) {
        dequeue_edge(tail);
    }
}

void delete_path(path *old) {
    delete_full_eq(&(old->edge_queue_tail));
    old->cost = 0;
    old->length = 0;
    old->start = NULL;

    free(old);
}

void print_path(path *p) {
    edge_queue *curr = p->edge_queue_tail->next_eq;

    printf("path length: %d\npath cost: %d\n\n", p->length, p->cost);

    while (curr != p->edge_queue_tail) {
        print_edge(curr->curr_edge);
        curr = curr->next_eq;
    }
    print_edge(curr->curr_edge);
}