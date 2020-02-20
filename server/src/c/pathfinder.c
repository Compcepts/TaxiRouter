/*
Implements cart path finding


METHODS:



*/

#include <stdlib.h>
#include <stdio.h>

#include "../defs/const.h"
#include "../defs/types.h"

#include "../h/path.h"
#include "../h/graph.h"

#include "../h/pathfinder.h"


static cart carts[CARTS];


static int cost_weight = 1;
static int length_weight = 1;



int main() {
    printf("Initializing graph\n");
    init_graph();

    printf("Finding vertex\n");
    carts[0].curr_loc = find_vertex(0,0);
    printf("Finding path\n");
    find_optimal_path(&(carts[0]), find_vertex(3,3));

    print_path(carts[0].curr_path);

    return 0;
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


/* Probably unnecessary */
bool append_path_vertex(path *p, vertex* v) {
    edge *e = find_edge(queue_head_edge(p)->dest, v);
    if (e != NULL) {
        enqueue_edge(&(p->edge_queue_tail), e);
        return TRUE;
    }
    return FALSE;
}


void set_path_weight(path *p, int c_index) {
    int run = 1;
    edge_queue *tail = p->edge_queue_tail, *next;

    if(tail != NULL) {
        if (tail == tail->next_eq) {
            set_weight(tail->curr_edge, c_index, run);
            set_weight(opposite_edge(tail->curr_edge), c_index, run);
        }
        else {
            next = tail->next_eq;
            while(next != tail) {
                set_weight(next->curr_edge, c_index, run);
                set_weight(opposite_edge(next->curr_edge), c_index, run);
                next = next->next_eq;
                run++;
            }
            set_weight(next->curr_edge, c_index, run);
            set_weight(opposite_edge(next->curr_edge), c_index, run);
        }
    }
}

vertex* get_destination(cart *c) {
    return queue_head_edge(c->curr_path)->dest;
}


bool traverse_path(cart *c) {
    path *p;
    edge *e;
    if (vertex_occupied(get_destination(c)) == FALSE) {
        p = c->curr_path;
        
        decrement_edges(p, c->index);

        e = dequeue_edge(&(c->curr_path->edge_queue_tail));

        set_vertex_occupation(e->src, FALSE);

        set_vertex_occupation(e->dest, TRUE);

        return TRUE;
    }
    return FALSE;
}

path_container* build_path_container(vertex *s, vertex *d) {
    path_container *ptc = empty_path_container();

    build_paths(ptc, ptc->curr_path, s, d, 1);

    return ptc;
}


bool edge_just_traveled(path *p, edge *e) {
    printf("made it");
    if (p == NULL) {
        return FALSE;
    }
    if (p->edge_queue_tail == NULL) {
        return FALSE;
    }
    if (p->edge_queue_tail->curr_edge == e) {
        return TRUE;
    }
    return FALSE;
}



/* NOT FUNCTIONAL --- THIS NEEDS MAJOR WORK */
void build_paths(path_container *ptc_head, path *p, vertex *s, vertex *d, int run) {
    edge *e, *poss_edges[4];
    vertex *curr_v = s, *directions[4];
    path *poss_paths[3];
    bool path_built = FALSE;

    printf("building path\n");

    int i = 0, j = 0, distances[4];

    for (i = 0; i < 4; i++) {
        poss_edges[i] = NULL;
        distances[i] = 100;
    }

    for (j = 0; j < 3; j++) {
        poss_paths[j] = NULL;
    }

    e = find_edge(curr_v, d);

    if (e == NULL) {
        directions[UP] = find_vertex(curr_v->coordx, curr_v->coordy+1);
        directions[RIGHT] = find_vertex(curr_v->coordx+1, curr_v->coordy);
        directions[DOWN] = find_vertex(curr_v->coordx, curr_v->coordy-1);
        directions[LEFT] = find_vertex(curr_v->coordx-1, curr_v->coordy);

        for (i = 0; i < 4; i++) {
            if (directions[i] != NULL) {
                distances[i] = distance(directions[i], d);
                poss_edges[i] = find_edge(curr_v, directions[i]);
            }
        }

        /* Checking distance cases */

        for (i = 0; i < 4; i++) {
            printf("first time here\n");


            if (distances[(i%4)] < distances[(i+2)%4] && distances[(i+1)%4] < distances[(i+3)%4]) {
                printf("second time here\n");
                if (edge_just_traveled(p, poss_edges[i]) == FALSE) {
                    printf("we are not making it here?!");
                    poss_paths[0] = p;
                    enqueue_edge(&(poss_paths[0]->edge_queue_tail), poss_edges[i]);
                    printf("we're fr not making it here");
                    build_paths(ptc_head, poss_paths[0], poss_edges[i]->dest, d, run+1);
                    path_built = TRUE;
                }
                if (edge_just_traveled(p, poss_edges[(i+1)%4]) == FALSE) {
                    if (path_built == TRUE) {
                        poss_paths[1] = copy_path(p);
                        enqueue_edge(&(poss_paths[1]->edge_queue_tail), poss_edges[(i+1)%4]);
                        insert_path(ptc_head, poss_paths[1]);
                        build_paths(ptc_head, poss_paths[1], poss_edges[(i+1)%4]->dest, d, run+1);
                    } else {
                        poss_paths[1] = p;
                        enqueue_edge(&(poss_paths[1]->edge_queue_tail), poss_edges[(i+1)%4]);
                        build_paths(ptc_head, poss_paths[1], poss_edges[(i+1)%4]->dest, d, run+1);
                    }
                }
            }

            else if (distances[(i%4)] < distances[(i+2)%4] && distances[(i+3)%4] < distances[(i+1)%4]) {
                printf("third time here\n");
                if (edge_just_traveled(p, poss_edges[i]) == FALSE) {
                    printf("should make it here\n");
                    poss_paths[0] = p;
                    print_edge(poss_edges[i]);
                    enqueue_edge(&(poss_paths[0]->edge_queue_tail), poss_edges[i]);
                    printf("como se dice here\n");
                    build_paths(ptc_head, poss_paths[0], poss_edges[i]->dest, d, run+1);
                    path_built = TRUE;
                }
                if (edge_just_traveled(p, poss_edges[(i+3)%4]) == FALSE) {
                    if (path_built == TRUE) {
                        poss_paths[2] = copy_path(p);
                        enqueue_edge(&(poss_paths[2]->edge_queue_tail), poss_edges[(i+3)%4]);
                        insert_path(ptc_head, poss_paths[2]);
                        build_paths(ptc_head, poss_paths[2], poss_edges[(i+3)%4]->dest, d, run+1);
                    } else {
                        poss_paths[2] = p;
                        enqueue_edge(&(poss_paths[2]->edge_queue_tail), poss_edges[(i+3)%4]);
                        build_paths(ptc_head, poss_paths[2], poss_edges[(i+3)%4]->dest, d, run+1);
                    }
                }
            }

            else if (distances[(i%4)] < distances[(i+2)%4] && distances[(i+1)%4] == distances[(i+3)%4]) {
                printf("fourth time here\n");
                if (distances[(i%4)] < distances[(i+2)%4] && distances[(i+1)%4] < distances[(i+3)%4]) {
                    if (edge_just_traveled(p, poss_edges[i]) == FALSE) {
                        poss_paths[0] = p;
                        enqueue_edge(&(poss_paths[0]->edge_queue_tail), poss_edges[i]);
                        build_paths(ptc_head, poss_paths[0], poss_edges[i]->dest, d, run+1);
                        path_built = TRUE;
                    }
                    if (edge_just_traveled(p, poss_edges[(i+1)%4]) == FALSE) {
                        if (path_built == TRUE) {
                            poss_paths[1] = copy_path(p);
                            enqueue_edge(&(poss_paths[1]->edge_queue_tail), poss_edges[(i+1)%4]);
                            insert_path(ptc_head, poss_paths[1]);
                            build_paths(ptc_head, poss_paths[1], poss_edges[(i+1)%4]->dest, d, run+1);
                        } else {
                            poss_paths[1] = p;
                            enqueue_edge(&(poss_paths[1]->edge_queue_tail), poss_edges[(i+1)%4]);
                            build_paths(ptc_head, poss_paths[1], poss_edges[(i+1)%4]->dest, d, run+1);
                            path_built = TRUE;
                        }
                    }
                    if (edge_just_traveled(p, poss_edges[(i+3)%4]) == FALSE) {
                        if (path_built == TRUE) {
                            poss_paths[2] = copy_path(p);
                            enqueue_edge(&(poss_paths[2]->edge_queue_tail), poss_edges[(i+3)%4]);
                            insert_path(ptc_head, poss_paths[2]);
                            build_paths(ptc_head, poss_paths[2], poss_edges[(i+3)%4]->dest, d, run+1);
                        } else {
                            poss_paths[2] = p;
                            enqueue_edge(&(poss_paths[2]->edge_queue_tail), poss_edges[(i+3)%4]);
                            build_paths(ptc_head, poss_paths[2], poss_edges[(i+3)%4]->dest, d, run+1);
                        }
                    }                    
                }
            }


            for (j = 0; j < 3; j++) {
                if (poss_paths[j] != NULL) {
                    poss_paths[j]->length++;
                    poss_paths[j]->cost += possible_collision(poss_paths[j]->edge_queue_tail->curr_edge, run);
                }
            }
        }


    } else {
        p->length++;
        p->cost += possible_collision(e, p->length);
        enqueue_edge(&(p->edge_queue_tail), e);
    }
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
    ptc->curr_path = p;

    return ptc;
}


path_container* empty_path_container() {
    path_container* ptc;

    ptc = (path_container*) malloc(sizeof(path_container));

    ptc->next_container = NULL;
    ptc->curr_path = empty_path();

    return ptc;
}


void insert_path(path_container *head, path *p) {
    path_container *next = head;
    while (next->next_container != NULL) {
        next = next->next_container;
    }
    next->next_container = new_path_container(p);
}

path* lowest_cost_path(path_container *ptc_head) {
    int lowest_cost = 1000;
    path_container *ptc = ptc_head, *lowest_ptc;
    path *p_ret;

    while (ptc != NULL) {
        if (path_cost(ptc->curr_path) < lowest_cost) {
            lowest_cost = path_cost(ptc->curr_path);
            p_ret = ptc->curr_path;
            lowest_ptc = ptc;
        }
        ptc = ptc->next_container;
    }
    lowest_ptc->curr_path = NULL;

    delete_full_ptc(ptc_head);

    return p_ret;
}



void delete_ptc(path_container *old) {
    if (old->curr_path != NULL) {
        delete_path(old->curr_path);
    }

    old->curr_path = NULL;
    old->next_container = NULL;

    free(old);
}

void delete_full_ptc(path_container *head) {
    while (head->next_container != NULL) {
        delete_full_ptc(head->next_container);
    }
    delete_ptc(head);
}



/* Cart methods */

cart* find_cart(int index) {
    return &(carts[index]);
}


/* Basic path cost function */
int path_cost(path *p) {
    return (cost_weight * p->cost) + (length_weight * p->length);
}