/*
Implements cart path finding


METHODS:



*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "../defs/const.h"
#include "../defs/types.h"

#include "../h/path.h"
#include "../h/graph.h"

#include "../h/pathfinder.h"


static cart carts[CARTS];


static int cost_weight = 1;
static int length_weight = 1;

static int creations = CARTS;

static int vertex_index = 0;

pthread_mutex_t path_lock, vertex_lock;

int passengers[PASSENGERS][2];


int main() {
    int i = 0;

    pthread_t t[CARTS];

    printf("Initializing graph\n");
    init_graph();

    printf("Finding paths\n");

    if (pthread_mutex_init(&path_lock, NULL) != 0) {
        printf("\nmutex 1 init failed\n");
        return 1;
    }

    if (pthread_mutex_init(&vertex_lock, NULL) != 0) {
        printf("\nmutex 2 init failed\n");
        return 1;
    }

    for (i = 0; i < CARTS; i++) {
        carts[i].curr_loc = find_vertex(0,0);
        pthread_create(&(t[i]), NULL, cart_handler, (void *) &(carts[i]));
    }

    pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    pthread_join(t[2], NULL);
    pthread_join(t[3], NULL);

    printf("\npaths made: %d\n\n\nPATHS:\n", creations);

    printf("\n\ncart 0 path:\n");
    printf("------------------------\n");
    print_path(carts[0].curr_path);
    printf("\n\ncart 1 path:\n");
    printf("------------------------\n");
    print_path(carts[1].curr_path);
    printf("\n\ncart 2 path:\n");
    printf("------------------------\n");
    print_path(carts[2].curr_path);
    printf("\n\ncart 3 path:\n");
    printf("------------------------\n");
    print_path(carts[3].curr_path);

    return 0;
}


void cart_handler(void *arg) {

    cart *c = (cart *) arg;
    vertex* d;

    pthread_mutex_lock(&vertex_lock);

    while (vertex_index < PASSENGERS) {

        d = find_vertex(passengers[vertex_index][0], passengers[vertex_index][1]);

        vertex_index++;

        pthread_mutex_unlock(&vertex_lock);

        pthread_mutex_lock(&path_lock);
        find_optimal_path(c, d);
        pthread_mutex_unlock(&path_lock);

        while (empty_eq(c->curr_path->edge_queue_tail) == FALSE) {
            if (traverse_path(c) == FALSE) {
                if (detect_gridlock() == TRUE) {
                    handle_gridlock();
                }
            }
        }

        c->curr_path->start = c->curr_loc;

        pthread_mutex_lock(&vertex_lock);
    }
}


void set_path_weight(path *p, int c_index) {
    int run = 1;
    edge_queue *curr = p->edge_queue_tail->next_eq;

    while (curr != p->edge_queue_tail) {
        set_weight(curr->curr_edge, run, c_index);
        set_weight(opposite_edge(curr->curr_edge), run, c_index);
        curr = curr->next_eq;
        run++;
    }
    set_weight(curr->curr_edge, run, c_index);
    set_weight(opposite_edge(curr->curr_edge), run, c_index);
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

        c->curr_loc = e->dest;

        return TRUE;
    }
    return FALSE;
}


bool detect_gridlock() {

}


void handle_gridlock() {
    
}



path_container* build_path_container(vertex *s, vertex *d) {
    path_container *ptc = empty_path_container();

    ptc->curr_path->start = s;

    printf("building path\n");

    build_paths(ptc, ptc->curr_path, d, 1);

    return ptc;
}


bool edge_just_traveled(path *p, edge *e) {
    if (p == NULL) {
        return FALSE;
    }
    if (p->edge_queue_tail == NULL) {
        return FALSE;
    }
    if (p->edge_queue_tail->curr_edge == opposite_edge(e)) {
        return TRUE;
    }
    return FALSE;
}



void build_paths(path_container *ptc_head, path *p, vertex *d, int run) {
    edge *e = NULL, *poss_edges[4];
    vertex *curr_v = next_vertex(p), *directions[4];
    path *poss_paths[3];
    bool path_built = FALSE;

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

            if (path_built == TRUE) {
                return;
            }

            else {

                if (distances[(i%4)] < distances[(i+2)%4] && distances[(i+1)%4] < distances[(i+3)%4]) {

                    if (edge_just_traveled(p, poss_edges[i]) == FALSE) {
                        if (path_built == TRUE) {
                            creations++;
                            poss_paths[0] = copy_path(p);
                            insert_path(ptc_head, poss_paths[0]);
                        }

                        else {
                            poss_paths[0] = p;
                            path_built = TRUE;
                        }
                    }

                    if (edge_just_traveled(p, poss_edges[(i+1)%4]) == FALSE) {
                        if (path_built == TRUE) {
                            creations++;
                            poss_paths[1] = copy_path(p);
                            insert_path(ptc_head, poss_paths[1]);
                        } 
                        
                        else {
                            poss_paths[1] = p;
                            path_built = TRUE;
                        }
                    }
                }




                else if (distances[(i%4)] < distances[(i+2)%4] && distances[(i+3)%4] < distances[(i+1)%4]) {

                    if (edge_just_traveled(p, poss_edges[i]) == FALSE) {
                        if (path_built == TRUE) {
                            creations++;
                            poss_paths[0] = copy_path(p);
                            insert_path(ptc_head, poss_paths[0]);
                        }
                        else {
                            poss_paths[0] = p;
                            path_built = TRUE;
                        }
                        
                    }

                    if (edge_just_traveled(p, poss_edges[(i+3)%4]) == FALSE) {
                        
                        if (path_built == TRUE) {
                            creations++;
                            poss_paths[2] = copy_path(p);
                            insert_path(ptc_head, poss_paths[2]);
                        } 
                        
                        else {
                            poss_paths[2] = p;
                            path_built = TRUE;
                        }
                    }
                }




                else if (distances[(i%4)] < distances[(i+2)%4] && distances[(i+1)%4] == distances[(i+3)%4]) {

                    if (edge_just_traveled(p, poss_edges[i]) == FALSE) {
                        if (path_built == TRUE) {
                            creations++;
                            poss_paths[0] = copy_path(p);
                            insert_path(ptc_head, poss_paths[0]);
                        }
                        else {
                            poss_paths[0] = p;
                            path_built = TRUE;
                        }
                    }

                    if (edge_just_traveled(p, poss_edges[(i+1)%4]) == FALSE) {
                        
                        if (path_built == TRUE) {
                            creations++;
                            poss_paths[1] = copy_path(p);
                            insert_path(ptc_head, poss_paths[1]);
                        } 
                        
                        else {
                            poss_paths[1] = p;
                            path_built = TRUE;
                        }
                    }

                    if (edge_just_traveled(p, poss_edges[(i+3)%4]) == FALSE) {
                        
                        if (path_built == TRUE) {
                            creations++;
                            poss_paths[2] = copy_path(p);
                            insert_path(ptc_head, poss_paths[2]);
                        } 
                        
                        else {
                            poss_paths[2] = p;
                            path_built = TRUE;
                        } 
                    }    
                }
                if (poss_paths[0] != NULL) {
                    enqueue_edge(&(poss_paths[0]->edge_queue_tail), poss_edges[i]);
                    poss_paths[0]->cost += possible_collision(poss_paths[0]->edge_queue_tail->curr_edge, run);
                    build_paths(ptc_head, poss_paths[0], d, run+1);
                }
                if (poss_paths[1] != NULL) {
                    enqueue_edge(&(poss_paths[1]->edge_queue_tail), poss_edges[(i+1)%4]);
                    poss_paths[1]->cost += possible_collision(poss_paths[1]->edge_queue_tail->curr_edge, run);
                    build_paths(ptc_head, poss_paths[1], d, run+1);
                }
                if (poss_paths[2] != NULL) {
                    enqueue_edge(&(poss_paths[2]->edge_queue_tail), poss_edges[(i+3)%4]);
                    poss_paths[2]->cost += possible_collision(poss_paths[2]->edge_queue_tail->curr_edge, run);
                    build_paths(ptc_head, poss_paths[2], d, run+1);
                }             
            }
        }

    } else {
        p->length = run;
        p->cost += possible_collision(e, run);
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
    int lowest_cost = 1000, i = 0;
    path_container *ptc = ptc_head, *lowest_ptc;
    path *p_ret;

    while (ptc != NULL) {
        if (path_cost(ptc->curr_path) < lowest_cost) {
            lowest_cost = path_cost(ptc->curr_path);
            p_ret = ptc->curr_path;
            lowest_ptc = ptc;
        }
        ptc = ptc->next_container;
        i++;
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
    if (head->next_container != NULL) {
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