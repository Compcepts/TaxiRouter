/*

Networking file for cart server. Handles information from carts and
make decisions. Sends appropriate decisions to carts.

Hevily based on: https://www.binarytides.com/socket-programming-c-linux-tutorial/

*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../defs/const.h"
#include "../defs/types.h"

#include "../h/path.h"
#include "../h/graph.h"

#include "../h/pathfinder.h"
#include "../h/network.h"



int main() {

    int i = 0;

    pthread_t t[CARTS];

    init_pathfinder();

    pthread_create(&(t[0]), NULL, route_cart, (void *) 0);
    pthread_create(&(t[1]), NULL, route_cart, (void *) 1);
    pthread_create(&(t[2]), NULL, route_cart, (void *) 2);
    pthread_create(&(t[3]), NULL, route_cart, (void *) 3);

    pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    pthread_join(t[2], NULL);
    pthread_join(t[3], NULL);


    return 0;
}


void route_cart(void *arg) {

    int i = (int) arg;

    struct sockaddr_n server, client;
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0), new_socket, c, port = 1234 + i;

    char mes[5][16] = {
    "stop",
    "drive",
    "turnleft",
    "turnright",
    "shutdown"
};

    if (socket_desc == -1) {
        printf("Could not open socket");

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

    c = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct socketaddr *) &client, (socklen_t *)&c);
    if (new_socket < 0) {
        perror("accept failed");

        return;
    }

    puts("accept successful");

    send(new_socket, mes, strlen(mes), 0);

    puts("message sent");

    close(socket_desc);

    return;

}
