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

void set_vertex_occupation(vertex *v, bool b);

bool vertex_occupied(vertex *v);

int distance(vertex *src, vertex *dest);

void print_edge(edge *e);

#endif