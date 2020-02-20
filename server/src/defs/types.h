#ifndef TYPES
#define TYPES


#include "./const.h"


typedef int bool;

typedef struct vertex {
    int coordx, coordy;
    bool occupied;
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

#endif