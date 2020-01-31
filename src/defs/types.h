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
    int weight;
} edge;

typedef struct graph {
    vertex vertices[VERT_ROADS][HORIZ_ROADS];
    edge edges[POSSIBLE_PATHS];
} graph;

typedef struct path {
    edge *curr_road;
    path *next_path;
} path;

enum activity_level {
    Open = 0,
    SevenTurns = 1,
    SixTurns = 2,
    FiveTurns = 3,
    FourTurns = 4,
    ThreeTurns = 5,
    TwoTurns = 6,
    OneTurn = 7,
};

#endif