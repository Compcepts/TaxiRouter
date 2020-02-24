#ifndef PATH
#define PATH

#include "../defs/const.h"
#include "../defs/types.h"

path* new_path(edge *e);

path* empty_path();

edge_queue* new_eq(edge *e);

void enqueue_edge(edge_queue **tail, edge *e);

edge* dequeue_edge(edge_queue **tail);

edge* queue_head_edge(path *p);

path* copy_path(path *p);

bool empty_eq(edge_queue *tail);

vertex* next_vertex(path *p);

void decrement_edges(path *p, int c);

void increment_edges(path *p, int c);

void add_cost(path *p, int c);

void add_length(path *p, int l);

void delete_eq(edge_queue *eq);

void delete_full_eq(edge_queue **tail);

void delete_path(path *old);

void print_path(path *p);

#endif