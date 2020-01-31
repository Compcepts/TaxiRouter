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

void set_vertex_occupied(vertex *v);

void set_vertex_unoccupied(vertex *v);

bool vertex_occupied(vertex *v);

int distance(vertex *src, vertex *dest);

#endif