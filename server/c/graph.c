/*

This file contains the methods for our basic state space model,
a weighted bidirectional graph shaped as a grid with m*n vertices.
Each edge has an opposing edge that together make up one road. For
this reason, weights of an edge are usually set the same as an
opposing edge as the road would be occupied by a given cart for
both edges. On the topic of weights, the weights themselves
have a somewhat nuanced meaning. Each edge has an individual
weight for each cart which represents the nummber of turns until
the cart is on that pair of edges (otherwise known as a road).
This allows us to make some informed predictions as to when cars
will reach certain roads thus allowing for some basic collisions
predictions. The following is a basic diagram of the graph:

     [0,                                           [m-1,
     n-1]                                          n-1]
     o________o________o________o         o________o
     |        |        |        |         |        |
     |[0,     |[1,     |[2,     |         |[m-2,   |
     |n-2]    |n-2]    |n-2]    |         |n-2]    |
     o________o________o________o . . . . o________o

     .        .        .                  .
     .        .        .                  .
     .        .        .                  .
     o________o________o________o         o________o
     |        |        |        |         |        |
     |        |        |        |         |[m-2,   |
     |[0, 2]  |[1, 2]  |[2, 2]  |         |2]      |
     o________o________o________o . . . . o________o
     |        |        |        |         |        |
     |        |        |        |         |[m-2,   |
     |[0, 1]  |[1, 1]  |[2, 1]  |         |1]      |
     o________o________o________o . . . . o________o
     |        |        |        |         |        |
     |        |        |        |         |[m-2,   |
     |[0, 0]  |[1, 0]  |[2, 0]  |         |0]      |
     o________o________o________o . . . . o________o [m-1, 0]


Each of these vertices can be accessed via x, y coordinates
and each edge can be accessed via source and destination
vertices (as these are directed edges). All vertex and edge
accesses are constant time, making the access/manipulation of
our state space very efficient. It should also be noted that
an edge can be used to access its opposite edge in constant
time as well.

*/



/* Standard libraries */
#include <stdlib.h>
#include <stdio.h>

/* User defined types and constants */
#include "../defs/types.h"
#include "../defs/const.h"

/* Method declarations, making the compiler happy */
#include "../h/graph.h"


/* Our state space will live on the stack during the execution
 * of our program. This works well for a road as we wont need
 * to add/remove roads very often. It also reduces the risk of
 * memory leakage */
static graph gr;


/* Initialize graph state space with "empty" values */

void init_graph() {
    int i, j, k, c;
    /* Initialize vertices */
    for (i = 0; i < VERT_ROADS; i++) {
        for (j = 0; j < HORIZ_ROADS; j++) {
            gr.vertices[i][j].coordx = i;
            gr.vertices[i][j].coordy = j;
            gr.vertices[i][j].busy = FALSE;
        }
    }

    /* Initialize edges */

    /* All edges from left to right */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i % ROADS_HORIZ;
        k = i / ROADS_HORIZ;

        gr.edges[i].src = &(gr.vertices[j][k]);
        gr.edges[i].dest = &(gr.vertices[j+1][k]);

        for (c = 0; c < CARTS; c++) {
            gr.edges[i].weight[c] = 0; 
        }
    }

    /* All edges from bottom to top */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i / ROADS_VERT;
        k = i % ROADS_VERT;

        gr.edges[i+EDGE_OFFSET].src = &(gr.vertices[j][k]);
        gr.edges[i+EDGE_OFFSET].dest = &(gr.vertices[j][k+1]);

        for (c = 0; c < CARTS; c++) {
            gr.edges[i+EDGE_OFFSET].weight[c] = 0;
        }
    }

    /* All edges from right to left */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i % ROADS_HORIZ;
        k = i / ROADS_HORIZ;

        gr.edges[i+(2*EDGE_OFFSET)].src = &(gr.vertices[j+1][k]);
        gr.edges[i+(2*EDGE_OFFSET)].dest = &(gr.vertices[j][k]);

        for (c = 0; c < CARTS; c++) {
            gr.edges[i+(2*EDGE_OFFSET)].weight[c] = 0;
        }
    }

    /* All edges from top to bottom */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i / ROADS_VERT;
        k = i % ROADS_VERT;

        gr.edges[i+(3*EDGE_OFFSET)].src = &(gr.vertices[j][k+1]);
        gr.edges[i+(3*EDGE_OFFSET)].dest = &(gr.vertices[j][k]);

        for (c = 0; c < CARTS; c++) {
            gr.edges[i+(3*EDGE_OFFSET)].weight[c] = 0;
        }
    }
}


/* Access an edge based on a source and destination vertex, returning
 * NULL if no edge exists between the two vertices */

edge* find_edge(vertex *s, vertex *d) {
    int x1, y1, x2, y2, index;

    if (s == NULL || d == NULL) {
        return NULL;
    }

    x1 = s->coordx;
    y1 = s->coordy;

    x2 = d->coordx;
    y2 = d->coordy;


    /* Dest is to right of src */
    if(x2 == x1 + 1 && y1 == y2) {
        index = (y1*ROADS_HORIZ) + x1;
    }
    /* Dest is above src */
    else if (y2 == y1 + 1 && x1 == x2) {
        index = EDGE_OFFSET + y1 + (x1*ROADS_VERT);
    }
    /* Dest is to left of src */
    else if (x1 == x2 +1 && y1 == y2) {
        index = (2*EDGE_OFFSET) + (y2*ROADS_HORIZ) + x2;
        
    }
    /* Dest is below src */
    else if(y1 == y2 + 1 && x1 == x2) {
        index = (3*EDGE_OFFSET) + y2 + (x2*ROADS_VERT);
    } 
    /* Two non-adjacent vertices input */
    else {
        return NULL;
    }

    return &(gr.edges[index]);
}


/* Flip the source and destination vertices of current edge to find
 * opposing edge, as this is the definition of oppposing edge */

edge* opposite_edge(edge *e) {
    return find_edge(e->dest, e->src);
}


/* Set the weight of the given index of the given edge */

void set_weight(edge *e, int w, int i) {
    e->weight[i] = w;
}


/* Find vertex from x, y coordinates */

vertex* find_vertex(int x, int y) {
    if (x <= ROADS_HORIZ && y <= ROADS_VERT && x >= 0 && y >= 0) {
        return &(gr.vertices[x][y]);
    }
    return NULL;
}


/* Return the weight of the given edge and given index */

int weight(edge *e, int i) {
    return e->weight[i];
}


/* Find the distance between two vertices, noting that this is not
 * Euclidian but instead the absolute value of the change in x plus
 * the absolute value of the change in y (this is a grid, and we
 * can only travel in a straight line if the two vertices are adjacent) */

int distance(vertex *src, vertex *dest) {
    return abs(src->coordx - dest->coordx) + abs(src->coordy - dest->coordy);
}


/* Format the edge data nicely and print */

void print_edge(edge *e) {
    if (e != NULL){
        printf("src: (%d, %d) -> dest: (%d, %d)\n", e->src->coordx, e->src->coordy, e->dest->coordx, e->dest->coordy);
    } else {
        printf("NULL edge\n");
    }
}
