/*

Basic implementation of weighted, bidirectional graph


METHODS:


*/
#include <stdlib.h>
#include <stdio.h>

#include "../defs/types.h"
#include "../defs/const.h"

#include "../h/graph.h"

static graph gr;

void init_graph() {
    int i, j, k, c;
    /* Initialize vertices */
    for (i = 0; i < VERT_ROADS; i++) {
        for (j = 0; j < HORIZ_ROADS; j++) {
            gr.vertices[i][j].coordx = i;
            gr.vertices[i][j].coordy = j;
            gr.vertices[i][j].occupied = FALSE;
        }
    }

    /* Initialize edges */
    /* First 20 roads from left to right */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i % ROADS_HORIZ;
        k = i / ROADS_HORIZ;

        gr.edges[i].src = &(gr.vertices[j][k]);
        gr.edges[i].dest = &(gr.vertices[j+1][k]);

        for (c = 0; c < CARTS; c++) {
            gr.edges[i].weight[c] = 0; 
        }
    }

    /* Next 20 roads from bottom to top */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i / ROADS_VERT;
        k = i % ROADS_VERT;

        gr.edges[i+EDGE_OFFSET].src = &(gr.vertices[j][k]);
        gr.edges[i+EDGE_OFFSET].dest = &(gr.vertices[j][k+1]);

        for (c = 0; c < CARTS; c++) {
            gr.edges[i+EDGE_OFFSET].weight[c] = 0;
        }
    }

    /* Next 20 roads from right to left */
    for (i = 0; i < (POSSIBLE_PATHS/4); i++) {
        j = i % ROADS_HORIZ;
        k = i / ROADS_HORIZ;

        gr.edges[i+(2*EDGE_OFFSET)].src = &(gr.vertices[j+1][k]);
        gr.edges[i+(2*EDGE_OFFSET)].dest = &(gr.vertices[j][k]);

        for (c = 0; c < CARTS; c++) {
            gr.edges[i+(2*EDGE_OFFSET)].weight[c] = 0;
        }
    }

    /* Last 20 roads from top to bottom */
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


edge* find_edge(vertex *s, vertex *d) {
    int x1, y1, x2, y2, index;

    x1 = s->coordx;
    y1 = s->coordy;

    x2 = d->coordx;
    y2 = d->coordy;


    /* Dest is to right of src */
    if(x2 == x1 + 1) {
        index = (y1*ROADS_HORIZ) + x1;
    }
    /* Dest is above src */
    else if (y2 == y1 + 1) {
        index = EDGE_OFFSET + y1 + (x1*ROADS_VERT);
    }
    /* Dest is to left of src */
    else if (x1 == x2 +1) {
        index = (2*EDGE_OFFSET) + (y2*ROADS_HORIZ) + x2;
        
    }
    /* Dest is below src */
    else if(y1 == y2 + 1) {
        index = (3*EDGE_OFFSET) + y2 + (x2*ROADS_VERT);
    } 
    /* Two non-adjacent vertices input */
    else {
        return NULL;
    }

    return &(gr.edges[index]);
}

edge* opposite_edge(edge *e) {
    return find_edge(e->dest, e->src);
}

void set_weight(edge *e, int w, int i) {
    e->weight[i] = w;
}

vertex* find_vertex(int x, int y) {
    if (x <= ROADS_HORIZ && y <= ROADS_VERT && x >= 0 && y >= 0) {
        return &(gr.vertices[x][y]);
    }
    return NULL;
}

int weight(edge *e, int i) {
    return e->weight[i];
}

void set_vertex_occupation(vertex *v, bool b) {
    v->occupied = b;
}

bool vertex_occupied(vertex *v) {
    return v->occupied;
}

int distance(vertex *src, vertex *dest) {
    return abs(src->coordx - dest->coordx) + abs(src->coordy - dest->coordy);
}

void print_edge(edge *e) {
    if (e != NULL) {
        printf("Source coordinates: x=%d, y=%d\nDestination coordinates: x=%d, y=%d\n", e->src->coordx, e->src->coordy, e->dest->coordx, e->dest->coordy);
    } else {
        printf("NULL edge\n");
    }
}
