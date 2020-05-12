#ifndef CONSTS
#define CONSTS

#define VERT_ROADS 4
#define HORIZ_ROADS 4

#define ROADS_VERT (VERT_ROADS-1)
#define ROADS_HORIZ (HORIZ_ROADS-1)

#define POSSIBLE_PATHS (2*((VERT_ROADS*ROADS_HORIZ)+(HORIZ_ROADS*ROADS_VERT)))

#define EDGE_OFFSET (POSSIBLE_PATHS/4)

#define CARTS 1

#define PASSENGERS 4

#define FALSE 0
#define TRUE 1

/*#define NULL ((void*)0)*/

enum direction {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

#endif
