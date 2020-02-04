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

typedef struct path {
    edge *curr_road;
    path *next_path;
} path;

typedef struct path_container {
    path *path_head;
    path_container *next_container;
} path_container;

typedef struct cart {
    vertex *curr_loc;
    path *curr_path;
    int index;
} cart;

#endif