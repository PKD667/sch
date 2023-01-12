#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#include "../include/python.h"
#include "../include/server.h"

#include "../include/cutils.h"

int sockfd;


void *handle_connection(void *threadarg);

void launch_server(int port, int adress) {
    struct sockaddr_in server_addr, client_addr; // Address structures for the server and client
    socklen_t addr_len; // Length of the address structure

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
    perror("Error creating socket");
    exit(1);
    }

    // Set up the address structure for the server
    memset(&server_addr, 0, sizeof(server_addr)); // Clear the structure
    server_addr.sin_family = AF_INET; // Use the IPv4 address family
    server_addr.sin_addr.s_addr = adress; // Listen on any available network interface
    server_addr.sin_port = htons(port); // Listen on the specified port
    dbg(1,"Before bind() call");
    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("Error binding socket");
    exit(1);
    }
    dbg(1,"Bind successful, continuing...");

    // Listen for incoming connections
    if (listen(sockfd, 1024) < 0) {
    perror("Error listening for connections");
    exit(1);
    }
    dbg(1,"Listening for connections on port %d...", port);

    connect:

    // Accept incoming connections
    addr_len = sizeof(client_addr);
    int newfd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_len);
    if (newfd < 0) {
    perror("Error accepting connection");
    exit(1);
    }
    dbg(1,"Accepted connection from %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    pthread_t thread;

    int rc = pthread_create(&thread, NULL, handle_connection, (void *) &newfd);
    if (rc) {
        perror("Error creating new thread");
        exit(1);
    }
    goto connect;

    close(sockfd);
    return;
}

void *handle_connection(void *threadarg) {

    int newfd = *((int *) threadarg);

    // At this point, you can communicate with the client using the newfd file descriptor
    while (1) {
        char buffer[1024];
        int n = read(newfd, buffer, 1024);
        if (n < 0 || n == 0) {
            msg(WARNING,"Client disconnected");
            // exit the thread
            return NULL;          
        }
        //printf("Received %d bytes: %s\n", n, buffer);

        // check if its an http request
        if (strncmp(buffer, "GET", 3) == 0) {
            msg(WARNING,"HTTP request, ignoring");
            continue;
        }

        // Handle the request
        const char* response = handle_request(buffer);

        if (response == NULL) {
            msg(WARNING,"Response is NULL, ignoring");
            continue;
        }

        n = write(newfd, response, strlen(response));
        if (n < 0) {
            perror("Error writing to socket");
            exit(1);
        }
    }

    close(newfd);
    pthread_exit(NULL);
}