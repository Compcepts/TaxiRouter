#ifndef GRAPH
#define GRAPH

#include "../defs/const.h"
#include "../defs/types.h"

void init_graph();

edge* find_edge(vertex *s, vertex *d);

edge* opposite_edge(edge *e);

void set_weight(edge *e, int w, int i);

vertex* find_vertex(int x, int y);

int weight(edge *e,int i);

void set_vertex_occupied(vertex *v);

void set_vertex_unoccupied(vertex *v);

bool vertex_occupied(vertex *v);

double distance(vertex *src, vertex *dest);

#endif