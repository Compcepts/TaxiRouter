/*

Basic implementation of weighted, bidirectional graph


METHODS:


*/

#include "../defs/types.h"
#include "../defs/const.h"
#include "../utils/math.h"

#include "../h/graph.h"

static graph gr;

void init_graph() {
    int i, j, k;
    /* Initialize vertices */
    for (i = 0; i < VERT_ROADS; i++) {
        for (j = 0; j < HORIZ_ROADS; i++) {
            gr.vertices[i][j].coordx = i;
            gr.vertices[i][j].coordy = j;
            gr.vertices[i][j].occupied = FALSE;
        }
    }

    /* Initialize edges */

    /* First 16 roads from left to right */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i % HORIZ_ROADS;
        k = i / VERT_ROADS;

        gr.edges[i].src = &(gr.vertices[j][k]);
        gr.edges[i].dest = &(gr.vertices[j+1][k]);

        gr.edges[i].weight = Open; 
    }

    /* Next 16 roads from bottom to top */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i % HORIZ_ROADS;
        k = i / VERT_ROADS;

        gr.edges[i+EDGE_OFFSET].src = &(gr.vertices[j][k]);
        gr.edges[i+EDGE_OFFSET].dest = &(gr.vertices[j][k+1]);

        gr.edges[i+EDGE_OFFSET].weight = Open; 
    }

    /* Next 16 roads from right to left */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i % HORIZ_ROADS;
        k = i / VERT_ROADS;

        gr.edges[i+(2*EDGE_OFFSET)].src = &(gr.vertices[j+1][k]);
        gr.edges[i+(2*EDGE_OFFSET)].dest = &(gr.vertices[j][k]);

        gr.edges[i+(2*EDGE_OFFSET)].weight = Open; 
    }

    /* Last 16 roads from top to bottom */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i % HORIZ_ROADS;
        k = i / VERT_ROADS;

        gr.edges[i+(3*EDGE_OFFSET)].src = &(gr.vertices[j][k+1]);
        gr.edges[i+(3*EDGE_OFFSET)].dest = &(gr.vertices[j][k]);

        gr.edges[i+(3*EDGE_OFFSET)].weight = Open; 
    }
}


edge* find_edge(vertex *s, vertex *d) {
    int x1, y1, x2, y2, index;

    x1 = (*s).coordx;
    y1 = (*s).coordy;

    x2 = (*d).coordx;
    y2 = (*d).coordy;

    if(x2 == x1 + 1) {
        index = (y1*4) + x1;
    }
    else if (y2 == y1 + 1) {
        index = EDGE_OFFSET + (y1*4) + x1;
    }
    else if (x1 == x2 +1) {
        index = (2*EDGE_OFFSET) + (y2*4) + x2;
        
    }
    else if(y1 == y2 + 1) {
        index = (3*EDGE_OFFSET) + (y2*4) + x2;
    } 
    else {
        return NULL;
    }

    return &(gr.edges[index]);
}

edge* opposite_edge(edge *e) {
    return find_edge(e->dest, e->src);
}

void set_weight(edge *e, int w) {
    (*e).weight = w;
}

vertex* find_vertex(int x, int y) {
    return &(gr.vertices[x][y]);
}

int weight(edge *e) {
    return (*e).weight;
}

void set_vertex_occupied(vertex *v) {
    v->occupied = TRUE;
}

void set_vertex_unoccupied(vertex *v) {
    v->occupied = FALSE;
}

bool vertex_occupied(vertex *v) {
    return v->occupied;
}

int distance(vertex *src, vertex *dest) {
    return euclidean_distance((*src).coordx, (*src).coordy, (*dest).coordx, (*dest).coordy);
}
