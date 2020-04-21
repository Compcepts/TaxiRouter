/*
Implements cart path finding


METHODS:



*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../defs/const.h"
#include "../defs/types.h"

#include "../h/path.h"
#include "../h/graph.h"

#include "../h/pathfinder.h"


static cart carts[CARTS];

static int cost_weight = 3;
static int length_weight = 1;

static int creations = CARTS;

static int vertex_index = 0;

static int connections = 0;

pthread_mutex_t path_lock, vertex_lock;

int passengers[PASSENGERS][2] = {
    {2, 5}, {0, 5}, {4, 5}, {2, 3},
    {3, 6}, {5, 7}, {1, 5}, {0, 7},
    {7, 2}, {4, 2}, {0, 1}, {4, 1},
    {5, 3}, {2, 6}, {1, 5}, {3, 7},
    {6, 3}, {7, 0}, {2, 1}, {6, 5},
    {2, 5}, {0, 5}, {4, 5}, {2, 3},
    {3, 6}, {5, 7}, {1, 5}, {0, 7},
    {7, 2}, {4, 2}, {0, 1}, {4, 1},
    {5, 3}, {2, 6}, {1, 5}, {3, 7},
    {6, 3}, {7, 0}, {2, 1}, {6, 5},
    {2, 5}, {0, 5}, {4, 5}, {2, 3},
    {3, 6}, {5, 7}, {1, 5}, {0, 7},
    {7, 2}, {4, 2}, {0, 1}, {4, 1},
    {5, 3}, {2, 6}, {1, 5}, {3, 7},
    {6, 3}, {7, 0}, {2, 1}, {6, 5},
    {2, 5}, {0, 5}, {4, 5}, {2, 3},
    {3, 6}, {5, 7}, {1, 5}, {0, 7},
    {7, 2}, {4, 2}, {0, 1}, {4, 1},
    {5, 3}, {2, 6}, {1, 5}, {3, 7},
    {6, 3}, {7, 0}, {2, 1}, {6, 5},
    {2, 5}, {0, 5}, {4, 5}, {2, 3},
    {3, 6}, {5, 7}, {1, 5}, {0, 7},
    {7, 2}, {4, 2}, {0, 1}, {4, 1},
    {5, 3}, {2, 6}, {1, 5}, {3, 7},
    {6, 3}, {7, 0}, {2, 1}, {6, 5}
};


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
        carts[i].curr_loc = find_vertex((i*2),(i*2));
        carts[i].index = i;
    }

    pthread_create(&(t[0]), NULL, cart_handler, (void *) &(carts[0]));
    pthread_create(&(t[1]), NULL, cart_handler, (void *) &(carts[1]));
    pthread_create(&(t[2]), NULL, cart_handler, (void *) &(carts[2]));
    pthread_create(&(t[3]), NULL, cart_handler, (void *) &(carts[3]));

    pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    pthread_join(t[2], NULL);
    pthread_join(t[3], NULL);


    return 0;
}


/*
void init_pathfinder() {

    int i;
   
    printf("Initializing graph\n");
    init_graph();

    printf("Finding paths\n");

    if (pthread_mutex_init(&path_lock, NULL) != 0) {
        printf("\nmutex 1 init failed\n");
        return;
    }

    if (pthread_mutex_init(&vertex_lock, NULL) != 0) {
        printf("\nmutex 2 init failed\n");
        return;
    }

    for (i = 0; i < CARTS; i++) {
        carts[i].curr_loc = find_vertex(0,0);
        carts[i].index = i;
    }
}*/


void cart_handler(void *arg) {

    cart *c = (cart*) arg;
    vertex *d, *v1, *v2;

    struct sockaddr_in server, client;
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0),
        new_socket,
        conn,
        port = 1234 + c->index;

    char mes[7][16] = {
    "stop",
    "drive",
    "shutdown",
    "left",
    "up",
    "right",
    "down"
};

    char *response = (char *) malloc(20*sizeof(char));

    if (socket_desc == -1) {
        puts("Could not open socket");

        return;
    }

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        puts("binding failed");

        return;
    }

    puts("binding done");

    listen(socket_desc, 3);

    puts("Waiting on connections...");

    conn = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct socketaddr *) &client, (socklen_t *)&conn);
    if (new_socket < 0) {
        perror("accept failed");

        return;
    }

    puts("accept successful");

    connections++;

    while (connections < CARTS) {
        sleep(1);
    }

    printf("%d cart starts\n", c->index);

    pthread_mutex_lock(&vertex_lock);
    pthread_mutex_lock(&(c->curr_loc->v_mut));

    while (vertex_index < PASSENGERS) {
        printf("%d cart taking its turn\n", c->index);
        d = find_vertex(passengers[vertex_index][0], passengers[vertex_index][1]);

        printf("passenger #: %d\n", vertex_index);

        vertex_index++;

        pthread_mutex_unlock(&vertex_lock);

        pthread_mutex_lock(&path_lock);
        find_optimal_path(c, d);

        printf("\nNewest path made:\n\n");
        printf("cart index: %d\n", c->index);


        /*print_path(c->curr_path);*/

        pthread_mutex_unlock(&path_lock);

        while (empty_eq(c->curr_path->edge_queue_tail) == FALSE) {
            puts("handling movement");
            while (strcmp(response, "stopped") != 0) {
                recv(new_socket, response, 20, 0);
            }
            strcpy(response, "null");


            puts("starting to move");
            switch (next_direction(c->curr_loc, queue_head_edge(c->curr_path))) {
                case 0:
                    send(new_socket, mes[3], strlen(mes[3]), 0);
                case 1:
                    send(new_socket, mes[4], strlen(mes[4]), 0);
                case 2:
                    send(new_socket, mes[5], strlen(mes[5]), 0);
                case 3:
                    send(new_socket, mes[6], strlen(mes[6]), 0);
            }

            while (strcmp(response, "turned") != 0) {
                recv(new_socket, response, 20, 0);
            }
            strcpy(response, "null");

            traverse_path(c);

            send(new_socket, mes[1], strlen(mes[1]), 0);
        }
        send(new_socket, mes[0], strlen(mes[0]), 0);

        c->curr_path->start = c->curr_loc;

        pthread_mutex_lock(&vertex_lock);
    }
    pthread_mutex_unlock(&(c->curr_loc->v_mut));
    pthread_mutex_unlock(&vertex_lock);

    send(new_socket, mes[2], strlen(mes[2]), 0);

    strcpy(response, "");
    free(response);

    printf("closing socket\n");
    close(socket_desc);
}



int next_direction(vertex *s, vertex *d) {
    int x_diff, y_diff;

    x_diff = d->coordx - s->coordx;
    y_diff = d->coordy - s->coordy;

    if (x_diff < 0) {
        return 0;
    }

    if (y_diff > 0) {
        return 1;
    }

    if (x_diff > 0) {
        return 2;
    }

    if (y_diff < 0) {
        return 3;
    }
}



bool cart_on_edge(cart *c) {
    int i = 0;
    edge *e = queue_head_edge(c->curr_path);
   
    for (i = 0; i < CARTS; i++) {
        if (weight(opposite_edge(e), i) == 1 && i != c->index) {
            return TRUE;
        }
    }
    return FALSE;
}



void zero_path_weight(path *p, int c_index) {
    edge_queue *curr = p->edge_queue_tail->next_eq;

    while (curr != p->edge_queue_tail) {
        set_weight(curr->curr_edge, 0, c_index);
        set_weight(opposite_edge(curr->curr_edge), 0, c_index);
        curr = curr->next_eq;
    }
    set_weight(curr->curr_edge, 0, c_index);
    set_weight(opposite_edge(curr->curr_edge), 0, c_index);
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


void traverse_path(cart *c) {
    edge *e;
    vertex *d = get_destination(c);

    pthread_mutex_unlock(&(c->curr_loc->v_mut));

    pthread_mutex_lock(&(d->v_mut));

    decrement_edges(c->curr_path, c->index);

    e = dequeue_edge(&(c->curr_path->edge_queue_tail));

    c->curr_loc = e->dest;
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
        if (weight(e, c) == w || weight(e, c) == w + 1 || weight(e, c) == w - 1) {
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
