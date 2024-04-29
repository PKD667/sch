#include "../include/server.h"
#include "../include/hashtable.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void _connect_();

void* args[][2] = {
    {"version",_print_version_}
};

char** args;

int launch_client() {
    // create a cli
    char input[1024];
    while (1) {
        printf("Enter a command: ");
        fgets(input, 1024, stdin);
        input[strlen(input) - 1] = '\0';
        if (strcmp(input, "exit") == 0) {
            break;
        }
        
    }
}

void _connect_()