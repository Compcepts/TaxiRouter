DEFS = ./defs/const.h ./defs/types.h ./h/path.h ./h/graph.h ./h/pathfinder.h

pathfinder: ./c/pathfinder.c 
	gcc -g -Wall -z execstack -o pathfinder ./c/graph.c ./c/path.c ./c/pathfinder.c $(DEFS)
