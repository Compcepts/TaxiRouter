/*

Networking file for cart server. Handles information from carts and
make decisions. Sends appropriate decisions to carts.

Hevily based on: https://www.binarytides.com/socket-programming-c-linux-tutorial/

*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    struct sockaddr_in server, client;
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0), new_socket, c;

    if (socket_desc == -1) {
        printf("Could not open socket");
    }

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(1234);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        puts("binding failed");
    }

    puts("binding done");

    listen(socket_desc, 3);

    puts("Waiting on connections...");

    c = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct socketaddr *) &client, (socklen_t *)&c);
    if (new_socket < 0) {
        perror("accept failed");
    }

    puts("accept successful");

    close(socket_desc);

    return 0;

}
