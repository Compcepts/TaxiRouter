/*
Implements cart path finding


METHODS:



*/

#include "../defs/const.h"
#include "../defs/types.h"

#include "../h/path.h"
#include "../h/graph.h"

#include "../h/pathfinder.h"


static cart carts[4];

path* construct_path(vertex *s, vertex *d) {
    edge *e = find_edge(s, d);
    path *p = new_path(e);
    return p;
}

bool append_path_vertex(path **p, vertex* v) {
    edge *e = find_edge((*p)->curr_road->dest, v);
    if (e != NULL) {
        path *p_new = new_path(e);
        append_path((*p), p_new);
        return TRUE;
    }
    return FALSE;
}

bool traverse_path(cart *c) {
    if (!vertex_occupied(c->curr_path->curr_road->dest)) {
        set_vertex_unoccupied(c->curr_path->curr_road->src);
        set_vertex_occupied((c->curr_path->curr_road->dest));

        c->curr_path = pop_head(&(c->curr_path));
        return TRUE;
    }
    return FALSE;
}
