/*

This file contains the highest level of abstraction of paths. It also
implements cart functionality, socket functionality, and serves as the
main entry point for the taxirouter. Freely speaking, this could be
seen as a mish-mash of the several concept of the taxirouter server
into one overly saturated file. A more developed approach would likely
have utilized another file whose sole purpose was to incorporate all
of the different server aspects, but instead here we are.

Let's break this up into three concepts developed in this file:

    1. Pathfinding
    2. Cart Handling
    3. Server Control Flow


I. PATHFINDING:

    The pathfinding implemented in this file is a simple recursive
    search through the road state space (graph) given a cart and a
    destination. The cart is important because its data will allow
    us the ability to have a starting location for paths, an index
    for path weights (more on this later), and a path queue pointer
    stored on the stack which we can use to reference dynamically
    allocated paths. To store paths being built, we use a path
    container - a dynamically allocated singly linked NULL terminated
    list. Each path container holds metadata on the path, that is
    the path's length and weight (projected collisions). The actual
    path building works as follows:

        i. Check all four possible directions of travel and store
        distances from correspoding vertex and destination
        ii. Starting with the edge on the left, we look for one of
        three cases:

            a. This edge and the edge 90 degrees clockwise are
            equidistant from the destination and both edges are
            closer than the edge parallel to them
            b. This edge and the edge 270 degrees clockwise are
            equidistant from the destination and both edges are
            closer than the edges parallel to them
            c. This edge is solely the closest to the destination
            and the two aforementioned edges are equidistant

        iii. If none of these are satisfied, we repeat, rotating
        our perspective by 90 degrees counter-clockwise
        iv. Once one of these conditions has been satisfied, we
        attempt to build down the two closest edges for cases (a)
        and (b), or three closest edges for case (c)
        v. However, we check to make sure that we did not just
        build down each of these edges:

            a. If we did, disregard this edge
            b. If we did not, check if we have already creates a
            new path in this process

                (i) If we have not, add this edge to the path
                (ii) If we have, copy the path creating a new
                dynamically allocated path identical to the path
                we began this process with. Add the edge to this
                path

        vi. Increment the number of edges in each path created
        vii. Increment the number of possible collisions for each
        path - this is done by checking the edge weights for the
        indices not belonging to the current cart. If the number
        of edges in the current path is equal or less than by 1
        to the weight, add 1 as this indicates another cart will
        likely be attempting to traverse down this edge in the same
        number of turns. It should be noted that we do not check
        the opposite edge as both the edge and opposite edge will
        contain the same value to indicate when a given cart will
        be on both of the edges.
        viii. Recursively call the build path function, this time
        with the destination vertex of the newest edge in the path
        as the source vertex of the process (still the same
        destination) unless the destination has already been reached

        ix. Once the destination has been reached for all possible
        paths, iterate through the path container list holding all
        of these paths and find the path with the lowest length
        and cost combination (although both values will actually
        be weighted differently when calculating this)
        x. Return the lowest cost path and store it as the cart's
        current path, deleting the other paths in the process


II. Cart Handling

    To the server, the cart is a device that will be allowed to
    update local data when the cart sends a coherent message in
    response to the commands it is given. The pair communicate
    as a kind of call and response system, The server observing
    local data such as which direction the next edge is or is
    it safe to traverse down an edge, then sending a corresponding
    command; the cart receiving the command processing as it sees
    fit, then letting the server know when it is done. The
    server does not need to know how the cart is handling driving,
    that is something the cart will handle locally. The commands
    sent to the carts are as follows:

        i. stop - indicates a path was successfully completed
        ii. drive - indicates an edge is safe to travel down
        iii. shutdown - indicates all passengers have been
        successfully transported
        iv. left - next vertex is to the left
        v. up - next vertex is up
        vi. right - next vertex is to the right
        vii. down - next vertex is down
        viii. avoid - another cart is traveling on the current edge,
        move out of the way

    The call and response flow control of the carts is as follows:

        ###############################
        #                             #
        #  find next direction, send  #/____
        #           to cart           #\    |
        #                             #     |
        ###############################     |
                       |                    |
                       |                    |
                       |                    |
                      \ /                   |
        ###############################     |
        #                             #     |
        #      Wait for "turned"      #     |
        #       acknowledgement       #     |
        #                             #     |
        ###############################     |
                       |                    |
                       |                    |
                       |                    |
                      \ /                   |
        ###############################     |
        #                             #     |
        #  Check travel queue to see  #     |
        #  if another cart owns edge  #     |
        #                             #     |
        ###############################     |
          / \  |               |            |
           |   | yes           | no         |
           |   |               |            |
           |  \ /             \ /           |
        #############     #############     |
        #           #     #           #     |
        #   avoid   #     #   drive   #     |
        #           #     #           #     |
        #############     #############     |
                               |            |
                               |            |
                               |            |
                              \ /           |
        ###############################     |
        #                             #     |
        #      Wait for "stopped"     #     |
        #       acknowledgement       #     |
        #                             #     |
        ###############################     |
                       |                    |
                       |                    |
                       |                    |
                      \ /                   |
        ###############################     |
        #                             #  no |
        #  Check to see if path has   #_____|
        #        been completed       #    / \
        #                             #     |
        ###############################     |
                       |                    |
                       | yes                |
                       |                    |
                      \ /                   |
                 ############               |
                 #          #               |
                 #   stop   #_______________|
                 #          #
                 ############

    This process will repeat for each individual cart until
    all passengers have been "trasnported"


III. SERVER CONTROL FLOW

    The main function of the server acts to initiate the road state
    space with empty values, initialize travel queues for each pair
    of edges, instantiate carts and their starting locations, prepare
    mutexes (as this is a multithreaded program), and finally create
    the threads which handle the carts. There will be CARTS cart
    handling threads where CARTS is an int defined in const.h. Each
    of these threads have the following control flow:

        i. Open TCP socket for communication with carts (there could
        be a single socket opened, but I am not sure how to go about
        handling this, so I opened one per thread where the port
        corresponds to the cart number)
        ii. Wait for connection from cart
        iii. Wait for CARTS connections to be made
        iv. Grab vertex of next passenger, incrememnting the passenger
        index (vertex_index) by one
        v. Create path for cart with this vertex as the destination
        vi. Proceed with cart handling as described in section (II)
        vii. Check if there are still passengers left to "transport"

            a. If there are, repeat from step (iv)
            b. If not, send shutdown message, close socket, and end
            thread

*/


/* Basic libraries for handling system-level tasks */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

/* Libraries enabling socket functionality */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

/* User defined constants and types */
#include "../defs/const.h"
#include "../defs/types.h"

/* Basic utility files for our state space */
#include "../h/path.h"
#include "../h/graph.h"
#include "../h/travel_queue.h"

/* Method declarations, making the compiler happy */
#include "../h/pathfinder.h"



/*
 * Pathfinder global variables:
 */

/* Stack cart objects */
static cart carts[CARTS];

/* Weights for calculating edge cost */
int cost_weight = 3;
int length_weight = 1;

/* Data collection variable for counting paths made */
int creations = CARTS;

/* Current passenger index */
int vertex_index = 0;

/* Established connections counter */
int connections = 0;

/* Locks to ensure passengers and path data are not accessed concurrently
 * when this would be innapropriate */
pthread_mutex_t path_lock, vertex_lock;

/* Passenger vertex array */
int passengers[PASSENGERS][2] = {
    {2, 3}, {1, 2}, {0, 0}, {3, 3}
};

/* Cart commands */
char mes[8][16] = {
    "stop",
    "drive",
    "shutdown",
    "left",
    "up",
    "right",
    "down",
    "avoid"
};

/* Main entry point of server */

int main() {

    int i = 0;

    pthread_t t[CARTS];

    printf("Initializing graph\n");

    /* Initialize state space */
    init_graph();
    init_travel_queue();

    printf("Finding paths\n");

    /* Mutex initialization */
    if (pthread_mutex_init(&path_lock, NULL) != 0) {
        printf("\nmutex 1 init failed\n");
        return 1;
    }
    if (pthread_mutex_init(&vertex_lock, NULL) != 0) {
        printf("\nmutex 2 init failed\n");
        return 1;
    }


    /* Cart initialization */
    for (i = 0; i < CARTS; i++) {
        printf("\ninitializing cart %d at [%d, %d]\n", i, i*2, i*2);
        carts[i].curr_loc = find_vertex((i*2),(i*2));
        carts[i].index = i;
    }

    /* Creation of cart handling threads */
    for (i = 0; i < CARTS; i++) {
        pthread_create(&(t[i]), NULL, cart_handler, (void *) &(carts[i]));
    }

    /* Waiting for completion of cart threads */
    for (i = 0; i < CARTS; i++) {
        pthread_join(t[i], NULL);
    }

    return 0;
}


/* Cart handling method as described above */

void cart_handler(void *arg) {

    cart *c = (cart*) arg;
    vertex *d, *v1, *v2;
    edge *curr_edge;

    /* Socket variables */
    struct sockaddr_in server, client;
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0),
        new_socket,
        conn,
        port = 1234 + c->index;


    char *response = (char *) malloc(20*sizeof(char));

    /* ---------------- SOCKET INTITIALIZATION ------------ */

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

    /* ---------------------------------------------------- */

    connections++;

    /* Wait for all carts to connect */

    while (connections < CARTS) {
        sleep(1);
    }

    printf("%d cart starts\n", c->index);

    pthread_mutex_lock(&vertex_lock);
    c->curr_loc->busy = TRUE;

    while (vertex_index < PASSENGERS) {

        /* ------------ FIND PATH ------------------ */

        d = find_vertex(passengers[vertex_index][0], passengers[vertex_index][1]);

        printf("passenger #: %d\n", vertex_index);

        vertex_index++;

        pthread_mutex_unlock(&vertex_lock);

        pthread_mutex_lock(&path_lock);
        find_optimal_path(c, d);

        printf("\nNewest path made:\n\n");
        printf("cart index: %d\n", c->index);


        print_path(c->curr_path);

        pthread_mutex_unlock(&path_lock);

        /* ----------------------------------------- */

        /* ----------- TRAVERSE PATH --------------- */

        while (empty_eq(c->curr_path->edge_queue_tail) == FALSE) {

            curr_edge = queue_head_edge(c->curr_path);

            /* TURN */
            puts("finding direction");
            switch (next_direction(c->curr_loc, curr_edge->dest)) {
                case LEFT:
                    send(new_socket, mes[3], strlen(mes[3]), 0);
                    break;
                case UP:
                    send(new_socket, mes[4], strlen(mes[4]), 0);
                    break;
                case RIGHT:
                    send(new_socket, mes[5], strlen(mes[5]), 0);
                    break;
                case DOWN:
                    send(new_socket, mes[6], strlen(mes[6]), 0);
                    break;
            }

            /* WAIT FOR "TURNED" */
            while (strcmp(response, "turned") != 0) {
                recv(new_socket, response, strlen("turned"), 0);
                response[6] = 0;
            }
            strcpy(response, "null");


            insert_tq(find_tq(curr_edge), c);

            /* CHECK IF CART OWNS EDGE */
            if (tq_head(find_tq(curr_edge)) != c) {

                /* AVOID */
                send(new_socket, mes[7], strlen(mes[7]), 0);
            }

            while (tq_head(find_tq(curr_edge)) != c) {
                sleep(1);
            }

            /* DRIVE */
            traverse_path(c);
            send(new_socket, mes[1], strlen(mes[1]), 0);

            /* WAIT FOR "STOPPED" */
            puts("handling movement");
            while (strcmp(response, "stopped") != 0) {
                recv(new_socket, response, strlen("stopped"), 0);
            }
            strcpy(response, "null");


            remove_tq(find_tq(curr_edge));
        }

        /* ----------------------------------------- */

        send(new_socket, mes[0], strlen(mes[0]), 0);

        c->curr_path->start = c->curr_loc;

        pthread_mutex_lock(&vertex_lock);
    }

    /* ------------- HANDLE SHUTDOWN ------------------- */

    pthread_mutex_unlock(&vertex_lock);

    send(new_socket, mes[2], strlen(mes[2]), 0);

    strcpy(response, "");
    free(response);

    printf("closing socket\n");
    close(socket_desc);

    /* ------------------------------------------------- */
}



/* Determine and return orientation of directed edge */

int next_direction(vertex *s, vertex *d) {
    int x_diff, y_diff;

    x_diff = d->coordx - s->coordx;
    y_diff = d->coordy - s->coordy;

    if (x_diff < 0) {
        return LEFT;
    }

    if (y_diff > 0) {
        return UP;
    }

    if (x_diff > 0) {
        return RIGHT;
    }

    if (y_diff < 0) {
        return DOWN;
    }
}



/* Helper function to determine if cart is on an edge, not utilized */

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



/* Reset weights on a carts path to prepare for switching paths,
 * not utilized*/

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


/* Set the weight for a cart's path by beginning at 1 for the first
 * edge, incrementing by 1 for each subsequent edge denoting how many
 * turns the cart will be at this edge by */

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


/* Return the destination vertex of cart's next edge */

vertex* get_destination(cart *c) {
    return queue_head_edge(c->curr_path)->dest;
}



/* Handle updating model data when cart traverses an edge:
 *
 *     i. Release starting vertex boolean (makeshift mutex),
 *     obtain destination vertex boolean (fake mutex)
 *     ii. Decrement path weights for cart
 *     iii. Pop edge from path
 *     iv. Adjust current location of cart
 */

void traverse_path(cart *c) {
    edge *e;
    vertex *d = get_destination(c);

    while (d->busy) {
        sleep(1);
    }

    c->curr_loc->busy = FALSE;

    d->busy = TRUE;

    decrement_edges(c->curr_path, c->index);

    e = dequeue_edge(&(c->curr_path->edge_queue_tail));

    c->curr_loc = e->dest;
}


/* Obtain a list of path containers, each holding a viable path */

path_container* build_path_container(vertex *s, vertex *d) {
    path_container *ptc = empty_path_container();

    ptc->curr_path->start = s;

    printf("building path\n");

    build_paths(ptc, ptc->curr_path, d, 1);

    return ptc;
}


/* Check to see if current edge is the opposite of the edge just added,
 * as we do not want to make paths going back and forth*/

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



/* Recursive path building method as described above */

void build_paths(path_container *ptc_head, path *p, vertex *d, int run) {
    edge *e = NULL, *poss_edges[4];
    vertex *curr_v = next_vertex(p), *directions[4];
    path *poss_paths[3];
    bool path_built = FALSE;

    int i = 0, j = 0, distances[4];

    for (i = 0; i < 4; i++) {
        poss_edges[i] = NULL;
        distances[i] = VERT_ROADS*HORIZ_ROADS;
    }

    for (j = 0; j < 3; j++) {
        poss_paths[j] = NULL;
    }

    /* Check if we can reach the destination from the current vertex */
    e = find_edge(curr_v, d);

    if (e == NULL) {

        /* Find vertices in all four directions */
        directions[UP] = find_vertex(curr_v->coordx, curr_v->coordy+1);
        directions[RIGHT] = find_vertex(curr_v->coordx+1, curr_v->coordy);
        directions[DOWN] = find_vertex(curr_v->coordx, curr_v->coordy-1);
        directions[LEFT] = find_vertex(curr_v->coordx-1, curr_v->coordy);

        /* If each direction has a valid vertex, update information for
         * that direction */
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



                /* CASE:      / \
                 *             |
                 *             |
                 *             |
                 * /___________|
                 * \
                 *  */

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



                /* CASE:    / \
                 *           |
                 *           |
                 *           |
                 *           |___________\
                 *                       /
                 *  */

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



                /* CASE:      / \
                 *             |
                 *             |
                 *             |
                 * /___________|____________\
                 * \                        /
                 *  */

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


                /* Build paths for satisfied case */
               
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

        /* Update path data for finalized path */

        p->length = run;
        p->cost += possible_collision(e, run);
        enqueue_edge(&(p->edge_queue_tail), e);
    }
}


/* Calculate number of projected collisions by checking weight indices of current edge.
 * Current cart index is unnecesary as a cart without a path will have 0's in all edge
 * weights at its index */

int possible_collision(edge *e, int w) {
    int c, count = 0;
    for (c = 0; c < CARTS; c++) {
        /* Both edges will have the same weight, so we do not need to check
        the opposite edge */
        if (weight(e, c) == w || weight(e, c) == w + 1) {
            count++;
        }
    }
    return count;
}



/* Set the given cart's path to the ideal path calculated by our path finding algorithm */

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



/* find the lowest cost path in the list of path containers,
 * remove it from the list, delete the path containers, and
 * return the lowest cost path*/

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
