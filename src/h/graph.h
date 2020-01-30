#ifndef GRAPH
#define GRAPH

#include "../defs/const.h"
#include "../defs/types.h"

void init_graph();

edge* find_edge(vertex *s, vertex *d);

edge* opposite_edge(edge *e);

void set_weight(edge *e, int w);

vertex* find_vertex(int x, int y);

int weight(edge *e);

int distance(vertex *src, vertex *dest);

#endif