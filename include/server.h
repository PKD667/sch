
extern int sockfd;

void launch_server(int port, int adress);
void *handle_connection(void *threadarg);