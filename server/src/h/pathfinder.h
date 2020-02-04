#ifndef PATHFINDER
#define PATHFINDER


path* construct_path(vertex *s, vertex *d);

bool append_path_vertex(path **p, vertex* v);

#endif