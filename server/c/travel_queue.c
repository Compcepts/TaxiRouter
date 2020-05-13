/*

Travel queues are designed as an auxillary data structure to
ensure that no two carts are driving on the same road at the
same time. The queue structure is the same as those of paths,
although the queues themselves can be referenced by an
pointer held on the stack which can be indexed by an edge.
These are not terribly important to the ideas of this project,
it's more a solution to a problem created by the simplistic
graph model. However, it should be noted that the travel queue
will store a pointer to the carts attempting to travel on a
road, so this is how we determine which cart travels next

 */


/* Standard libraries */
#include <stdlib.h>
#include <stdio.h>

/* User defined types and constants */
#include "../defs/const.h"
#include "../defs/types.h"

/* Include basic graph functionality */
#include "../h/graph.h"

/* Method declarations, make the compiler happy */
#include "../h/travel_queue.h"



/* The travel queues will be stored on the stack as each one
 * corresponds to a stack-allocated pair of edges */

static tq_tail tails[POSSIBLE_PATHS/2];


/* Iniatialize all travel queues with NULL values */

void init_travel_queue() {
    int i = 0;

    for (i = 0; i < POSSIBLE_PATHS/2; i++) {
        tails[i].tail = NULL;
    }
}


/* Find a travel queue based on its corresponding edge, this
 * accounts for opposite edges as well */

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


/* Enqueue a cart into the travel queue */

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


/* Dequeue from travel queues */

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


/* Return the cart at the head of the travel queue */

cart* tq_head(tq_tail *tail) {
    if (tail->tail == NULL) {
        return NULL;
    }

    else {
        return tail->tail->next_tq->q_cart;
    }
}


/* Dynamically allocate new travel queue element */

travel_queue* new_tq(cart *c) {
    travel_queue *tq = (travel_queue *) malloc(sizeof(travel_queue));

    tq->next_tq = NULL;
    tq->q_cart = c;

    return tq;
}


/* Delete travel queue element */

void delete_tq(travel_queue *tq) {
    tq->q_cart = NULL;
    tq->next_tq = NULL;

    free(tq);
}
