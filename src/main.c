
/*
This is the main file of the first implementation of an SCH (Secure Communist Host) server.

Copyright (C) 2019-2020 by pkd@sovietlinux.ml.
*/

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

#include "../include/server.h"
#include "../include/python.h"

#include "../include/cutils.h"

// Signal handler for SIGINT
void sigint_handler(int signum) {
  msg(ERROR,"Caught SIGINT, exiting");
  // Close the server socket
  close(sockfd);
  // close shared memory
  shmctl(co_shmid, IPC_RMID, NULL);
  exit(0);
}

int main(int argc, char *argv[]) {
  
  // get args 
  if (argc < 2) {
    printf("Usage: %s port [adress]\n", argv[0]);
    return 1;
  }
  int port = atoi(argv[1]);
  int adress = INADDR_ANY;
  if (argc > 2) {
    adress = atoi(argv[2]);
  }

  DEBUG = 3;

  // Set up the signal handler for SIGINT
  struct sigaction action;
  action.sa_handler = sigint_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGINT, &action, NULL);


  //init python
  load_python_module("sch");

  // Create the server socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Error creating socket");
    exit(1);
  }
  launch_server(port, adress);
  return 0;
}