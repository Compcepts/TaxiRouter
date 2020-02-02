/*
Implements cart path finding


METHODS:



*/

#include <stdlib.h>

#include "../defs/const.h"
#include "../defs/types.h"

#include "../h/path.h"
#include "../h/graph.h"

#include "../h/pathfinder.h"


static cart carts[CARTS];



int main() {
    init_graph();

    carts[0].curr_loc = find_vertex(0,0);
    find_optimal_path(&(carts[0]), find_vertex(3,3));

    return 0;
}

void init_pathfinder() {
    init_graph();


}

/* Probably unnecessary */
path* construct_path(vertex *s, vertex *d) {
    edge *e = find_edge(s, d);
    if (e != NULL) {
        path *p = new_path(e);
        return p;
    }
    return NULL;
}



bool append_path_vertex(path *p, vertex* v) {
    edge *e = find_edge(p->curr_road->dest, v);
    if (e != NULL) {
        path *p_new = new_path(e);
        append_path(p, p_new);
        return TRUE;
    }
    return FALSE;
}


void set_path_weight(path *p, int c_index) {
    path *curr_p = p;
    int run = 1;
    while (curr_p != NULL) {
        set_weight(p->curr_road, run, c_index);
        set_weight(opposite_edge(p->curr_road), run, c_index);
        run++;
        curr_p = curr_p->next_path;
    }
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

path_container* build_path_container(vertex *s, vertex *d) {
    path_container *ptc = empty_path_container();

    build_paths(ptc, ptc->path_head, s, d);

    return ptc;
}


/* NOT FUNCTIONAL --- THIS NEEDS MAJOR WORK */
void build_paths(path_container *ptc, path *p_head, vertex *s, vertex *d) {
    edge *e = NULL;
    vertex *up, *right, *down, *left;
    path *curr_p = p_head, *p;

    int up_dist, right_dist, down_dist, left_dist;

    while (curr_p->next_path != NULL) {
        curr_p = curr_p->next_path;
    }

    e = find_edge(s, d);

    if (e == NULL) {
        up = find_vertex(s->coordx, s->coordy+1);
        right = find_vertex(s->coordx+1, s->coordy);
        down = find_vertex(s->coordx, s->coordy-1);
        left = find_vertex(s->coordx-1, s->coordy);
    }
    
    p = new_path(e);
    append_path(curr_p, p);
    add_path(ptc, p_head);
}


int possible_collision(edge *e, int w) {
    int c, count = 0;
    for (c = 0; c < CARTS; c++) {
        /* Both edges will have the same weight, so we do not need to check
        the opposite edge */
        if (weight(e, c) == w) {
            count++;
        }
    }
    return count;
}


void find_optimal_path(cart *c, vertex *d) {
    c->curr_path = lowest_cost_path(build_path_container(c->curr_loc, d));
    set_path_weight(c->curr_path, c->index);
}














/* Path container methods */

path_container* new_path_container(path *p) {
    path_container* ptc;

    ptc = (path_container*) malloc(sizeof(path_container));

    ptc->next_container = NULL;
    ptc->path_head = p;

    return ptc;
}


path_container* empty_path_container() {
    path_container* ptc;

    ptc = (path_container*) malloc(sizeof(path_container));

    ptc->next_container = NULL;
    ptc->path_head = NULL;

    return ptc;
}


void add_path(path_container *ptc, path *p) {
    ptc->path_head = p;
}


void append_ptc(path_container *tail, path_container *next) {
    tail->next_container = next;
}


path* lowest_cost_path(path_container *ptc) {
    path *lowest_cost, *curr_p = ptc->path_head, *p_head;
    path_container *curr_ptc = ptc;
    int current_low = 100, run = 1, running_sum = 0;

    while (curr_ptc != NULL) {
        running_sum = 0;
        run = 1;
        p_head = curr_ptc->path_head;
        curr_p = p_head;
        while (curr_p != NULL) {
            running_sum += possible_collision(curr_p->curr_road, run);
            running_sum++;
            run++;
            curr_p = curr_p->next_path;
        }
        if (running_sum < current_low) {
            lowest_cost = p_head;
        }
        curr_ptc = curr_ptc->next_container;
    }

    return extract_path(&ptc, lowest_cost);
}


path* extract_path(path_container *ptc, path *p) {
    path_container *current_ptc = ptc;
    while(current_ptc->path_head != p) {
        if (current_ptc->next_container != NULL) {
            current_ptc = current_ptc->next_container;
        } else {
            return NULL;
        }
    }
    current_ptc->path_head = NULL;
    delete_full_ptc(&ptc);

    return p;
}


void delete_ptc(path_container **old) {
    if ((*old)->path_head != NULL) {
        delete_full_path(&((*old)->path_head));
    }

    (*old)->path_head = NULL;
    (*old)->next_container = NULL;

    free(*old);
}

void delete_full_ptc(path_container **head) {
    while((*head)->next_container != NULL) {
        delete_full_path(&(*head)->next_container);
    }
    delete_ptc(head);
}



/* Cart methods */

cart* find_cart(int index) {
    return &(carts[index]);
}