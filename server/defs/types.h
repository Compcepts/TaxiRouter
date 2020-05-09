#ifndef TYPES
#define TYPES


#include "./const.h"
#include <pthread.h>


typedef char bool;

typedef struct vertex {
    int coordx, coordy;
    bool busy;
} vertex;

typedef struct edge {
    vertex *src, *dest;
    int weight[CARTS];
} edge;

typedef struct graph {
    vertex vertices[VERT_ROADS][HORIZ_ROADS];
    edge edges[POSSIBLE_PATHS];
} graph;

typedef struct edge_queue {
    edge *curr_edge;
    struct edge_queue *next_eq;
} edge_queue;

typedef struct path {
    int length;
    int cost;
    vertex *start;
    struct edge_queue *edge_queue_tail;
} path;

typedef struct path_container {
    path *curr_path;
    struct path_container *next_container;
} path_container;

typedef struct cart {
    vertex *curr_loc;
    path *curr_path;
    int index;
} cart;

typedef struct travel_queue {
    cart *q_cart;
    struct travel_queue *next_tq;
} travel_queue;

typedef struct tq_tail {
    travel_queue *tail;
} tq_tail;

#endif
