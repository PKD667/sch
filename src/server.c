#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>


#include "../include/python.h"
#include "../include/server.h"

#include "../include/cutils.h"

int sockfd;

int co_shmid;

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

    // now do the memory stuff
    // Create shared memory segment
    co_shmid = shmget(IPC_PRIVATE, sizeof(conn_list), IPC_CREAT | 0666);
    if (co_shmid < 0) {
        perror("Error creating shared memory segment");
        exit(1);
    }

    // Attach shared memory segment to the server process
    conn_list *list = shmat(co_shmid, NULL, 0);
    if (list == (void*)-1) {
        perror("Error attaching shared memory segment");
        exit(1);
    }

    printf(" list = %p\n",list);
    printf("shmid = %d\n",co_shmid);
    

    connect:

    // Accept incoming connections
    addr_len = sizeof(client_addr);
    int newfd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_len);
    if (newfd < 0) {
    perror("Error accepting connection");
    exit(1);
    }
    msg(INFO,"Accepted connection from %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Cconnection struct
    connection* client_connection = &list->connections[list->connections_num++];
    client_connection->socket = newfd;
    client_connection->address = client_addr;

    printf(" list->connections_num = %d\n",list->connections_num);


    pthread_t thread;

    int rc = pthread_create(&thread, NULL, handle_connection, (void *) client_connection);
    if (rc) {
        perror("Error creating new thread");
        exit(1);
    }
    goto connect;
    shmctl(co_shmid, IPC_RMID, NULL);
    close(sockfd);
    return;
}

void *handle_connection(void *threadarg) {

    connection* conn = (connection*) threadarg;

    msg(INFO,"New connection thread started, wainting for INIT call...");
    // wait for an INIT call
    char buffer[1024];
    int n = read(conn->socket, buffer, 1024);
    printf("Received %d bytes: %s\n", n, buffer);
    if (n < 0 || n == 0) {
        msg(WARNING,"Client disconnected");
        // exit the thread
        return NULL;          
    }
    // check for http spam 
    if (!strncmp(buffer, "{", 1)) {
        msg(WARNING,"SPAM request, ignoring");
        return NULL;
    }

    PyDictObject* init_res = handle_request(buffer, conn);
    if (init_res == NULL) {
        msg(ERROR,"INIT call failed, exiting thread");
        close(conn->socket);
        return NULL;
    }
    dbg(1,"Request sucessfully handled");


    // get init_res["headers"]["Connection-ID]
    PyObject* connection_id = PyDict_GetItemString(PyDict_GetItemString((PyObject*)init_res, "headers"), "Connection-ID");
    strcpy(conn->cuuid,PyUnicode_AsUTF8(connection_id));
    // get init_res["headers"]["Client-Name]
    PyObject* client_name = PyDict_GetItemString(PyDict_GetItemString((PyObject*)init_res, "headers"), "Client-Name");
    strcpy(conn->client_name,PyUnicode_AsUTF8(client_name));
    dbg(1,"Connection-ID: %s", conn->cuuid);
    dbg(1,"Client-Name: %s", conn->client_name);

    // free memory
    Py_DECREF(init_res);
    Py_DECREF(connection_id);
    Py_DECREF(client_name);

    msg(INFO,"INIT call received (CUUID = %s), starting loop...", conn->cuuid);


    // At this point, you can communicate with the client using the newfd file descriptor
    while (1) {
        char buffer[1024];
        int n = read(conn->socket, buffer, 1024);
        if (n < 0 || n == 0) {
            msg(WARNING,"Client disconnected");
            // exit the thread
            return NULL;          
        }
        printf("Received %d bytes: %s\n", n, buffer);

    }

    close(conn->socket);
    pthread_exit(NULL);
}