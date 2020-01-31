/*
Implements cart path finding


METHODS:



*/

#include "../defs/const.h"
#include "../defs/types.h"

#include "../h/path.h"
#include "../h/graph.h"


init_graph();





path* construct_path(vertex *s, vertex *d) {
    edge *e = find_edge(s, d);
    path *p = new_path(e);
    return p;
}