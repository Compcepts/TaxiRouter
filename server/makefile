DEFS = ./defs/const.h ./defs/types.h ./h/path.h ./h/graph.h ./h/pathfinder.h ./h/travel_queue.h

pathfinder: ./c/pathfinder.c 
	gcc -g -lpthread -Wall -o pathfinder ./c/graph.c ./c/path.c ./c/travel_queue.c ./c/pathfinder.c $(DEFS)
