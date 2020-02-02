/*
Implements cart path finding


METHODS:



*/

#include "../defs/const.h"
#include "../defs/types.h"

#include "../h/path.h"
#include "../h/graph.h"

#include "../h/pathfinder.h"


static cart carts[CARTS];


/* Probably unnecessary */
path* construct_path(vertex *s, vertex *d) {
    edge *e = find_edge(s, d);
    if (e != NULL) {
        path *p = new_path(e);
        return p;
    }
    return NULL;
}


/* Probably unnecesary */
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
    path *p;
    if (vertex_occupied(c->curr_path->curr_road->dest) == FALSE) {
        set_vertex_unoccupied(c->curr_path->curr_road->src);

        c->curr_loc = c->curr_path->curr_road->dest;
        set_vertex_occupied((c->curr_path->curr_road->dest));

        p = c->curr_path;

        while (p != NULL) {
            set_weight(p->curr_road, weight(p->curr_road, c->index)-1, c->index);
            set_weight(opposite_edge(p->curr_road), weight(opposite_edge(p->curr_road), c->index)-1, c->index);
            p = p->next_path;
        }

        c->curr_path = pop_path_head(&(c->curr_path));
        return TRUE;
    }
    return FALSE;
}

path* build_path(vertex *s, vertex *d, int run, int c_index) {
    vertex *up, *down, *left, *right;
    bool col;
    path *p_final;
    edge *e = NULL;

    /* Give distance extreme value to ensure that, if its corresponding vertex is non-existant,
    that direction is not traveled */
    double up_dist = 1000, down_dist = 1000, left_dist = 1000, right_dist = 1000;

    up = find_vertex(s->coordx, s->coordy + 1);
    down = find_vertex(s->coordx, s->coordy - 1);
    right = find_vertex(s->coordx + 1, s->coordy);
    left = find_vertex(s->coordx - 1, s->coordy);
    
    if (up == d) {
        e = find_edge(up, d);
    }
    else if (right == d) {
        e = find_edge(right, d);
    }
    else if (down == d) {
        e = find_edge(down, d);
    }
    else if (left == d) {
        e = find_edge(left, d);
    } 

    else if (e == NULL) {
        if (up != NULL) {
            up_dist = distance(up, d);
        }
        if (down != NULL) {
            down_dist = distance(down, d);
        }
        if (right != NULL) {
            right_dist = distance(right, d);
        }
        if (left != NULL) {
            left_dist = distance(left, d);
        }

        /*
        RULES OF TRAVEL:
            1. Never travel highest distance direction
            2. Check remaining possible paths up to a depth of 3
            3. Add path distance traveled, distance to target, and collisions, choose lowest value
        */

       e = predicted_edge();
    }



    p_final = new_path(e);

    set_weight(e, run, c_index);
    set_weight(opposite_edge(e), run, c_index);

    return p_final;
}


bool check_collision(edge *e, int w) {
    int c;
    for (int c = 0; c < CARTS; c++) {
        if (weight(e, c) == w) {
            return TRUE;
        }
        if (weight(opposite_edge(e), c) == w) {
            return TRUE;
        }
    }
    return FALSE;
}


path* find_optimal_path(cart *c, vertex *d) {
    int run = 1;
    vertex *start = c->curr_loc;
    path *tail;

    c->curr_path = build_path(c->curr_loc, d, run, c->index);

    tail = c->curr_path;

    while (c->curr_path->curr_road->dest != d) {
        tail->next_path = build_path(tail->curr_road->dest, d, run, c->index);
        tail = tail->next_path;
        run++;
    }
}