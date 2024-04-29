#pragma once

#include <netinet/in.h>

extern int sockfd;

typedef struct {
    int socket;
    char cuuid[37];
    char client_name[256];
    struct sockaddr_in address;
} connection;

typedef struct {
    int server_socket;
    int connections_num;
    connection connections[64];
} conn_list;

extern conn_list list[20];


void launch_server(int port, int adress);
void *handle_connection(void *threadarg);