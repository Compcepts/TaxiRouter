#ifndef PATH
#define PATH

#include "../defs/const.h"
#include "../defs/types.h"


path* new_path(edge *e);

path* empty_path();

void append_path(path *begin, path *end);

path* copy_path(path *p);

path* pop_path_head(path **head);

void delete_path(path **old);

void delete_full_path(path **head);

#endif