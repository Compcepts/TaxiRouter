#ifndef PATHFINDER
#define PATHFINDER

#include "../defs/const.h"
#include "../defs/types.h"

path* construct_path(vertex *s, vertex *d);

bool append_path_vertex(path *p, vertex* v);

void set_path_weight(path *p, int c_index);

vertex* get_destination(cart *c);

bool traverse_path(cart *c);

path_container* build_path_container(vertex *s, vertex *d);

bool edge_just_traveled(path *p, edge *e);

void build_paths(path_container *ptc_head, path *p, vertex *s, vertex *d, int run);

int possible_collision(edge *e, int w);

void find_optimal_path(cart *c, vertex *d);

path_container* new_path_container(path *p);

path_container* empty_path_container();

void insert_path(path_container *head, path *p);

path* lowest_cost_path(path_container *ptc_head);

void delete_ptc(path_container *old);

void delete_full_ptc(path_container *head);

cart* find_cart(int index);

int path_cost(path *p);

#endif