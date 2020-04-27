#ifndef TRAVELQUEUE
#define TRAVELQUEUE

#include "../defs/const.h"
#include "../defs/types.h"

void init_travel_queue();

tq_tail* find_tq(edge *e);

void insert_tq(tq_tail *tail, cart *c);

void remove_tq(tq_tail *tail);

cart* tq_head(tq_tail *tail);

travel_queue* new_tq(cart *c);

void delete_tq(travel_queue *tq);

#endif
