



#include <stdlib.h>
#include <stdio.h>

#include "../defs/const.h"
#include "../defs/types.h"

#include "../h/graph.h"
#include "../h/travel_queue.h"

static tq_tail tails[POSSIBLE_PATHS/2];


void init_travel_queue() {
    int i = 0;

    for (i = 0; i < POSSIBLE_PATHS/2; i++) {
        tails[i].tail = NULL;
    }
}


tq_tail* find_tq(edge *e) {
    vertex *s = e->src, *d = e->dest;
    edge *opp_e = opposite_edge(e);

    int index;

    if (s->coordy > d->coordy || s->coordx > d->coordx) {
        s = opp_e->src;
        d = opp_e->dest;
    }

    if (s->coordx < d->coordx) {
        index = (s->coordy*ROADS_HORIZ) + s->coordx;
    }

    else if (s->coordy < d->coordy) {
        index = EDGE_OFFSET + (s->coordx*ROADS_VERT) + s->coordy;
    }

    return &(tails[index]);
}


void insert_tq(tq_tail *tail, cart *c) {
    travel_queue *tq = new_tq(c), *old_tail = tail->tail;

    if (old_tail == NULL) {
        tail->tail = tq;
        tq->next_tq = tq;
    }

    else if (old_tail->next_tq == old_tail) {
        tail->tail = tq;
        tq->next_tq = old_tail;
        old_tail->next_tq = tq;
    }

    else {
        tq->next_tq = old_tail->next_tq;
        old_tail->next_tq = tq;
        tail->tail = tq;
    }
}



void remove_tq(tq_tail *tail) {
    travel_queue *tq_tail = tail->tail, *rem_tq;

    if (tq_tail == NULL) {
        return;
    }

    else if (tq_tail->next_tq == tq_tail) {
        delete_tq(tq_tail);
        tail->tail = NULL;
    }

    else {
        rem_tq = tq_tail->next_tq;
        tq_tail->next_tq = tq_tail->next_tq->next_tq;
        delete_tq(rem_tq);
    }
}



cart* tq_head(tq_tail *tail) {
    if (tail->tail == NULL) {
        return NULL;
    }

    else {
        return tail->tail->next_tq->q_cart;
    }
}



travel_queue* new_tq(cart *c) {
    travel_queue *tq = (travel_queue *) malloc(sizeof(travel_queue));

    tq->next_tq = NULL;
    tq->q_cart = c;

    return tq;
}


void delete_tq(travel_queue *tq) {
    tq->q_cart = NULL;
    tq->next_tq = NULL;

    free(tq);
}
