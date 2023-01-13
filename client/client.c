#include "../include/server.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s shmid\n", argv[0]);
        return 1;
    }
    int shmid = atoi(argv[1]);
    conn_list *list = shmat(shmid, NULL, 0);
    if (list == (void*)-1) {
        perror("Error attaching shared memory segment");
        return 1;
    }


    printf(" list = %p\n",list);
    printf(" list->connections_num = %d\n",list->connections_num);
    // loop through the connections
    for (int i = 0; i < list->connections_num; i++) {
        connection *conn = &list->connections[i];
        printf("Connection %d: %s:%d\n", i, inet_ntoa(conn->address.sin_addr), ntohs(conn->address.sin_port));
    }

    if (shmdt(list) == -1) {
        perror("Error detaching shared memory segment");
    }
    return 0;
}
