#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>
#include <pthread.h>
#include "../common/list.h"


int start_server(char *hostname, char *port, int backlog);  // start the server
int accept_client(int serv_sock);                    // accept a connection from client
void send_acknowledgement(int client_sock_fd, int ack_type);
void disconnect_client(Session *session); 
// helper functions
void *get_in_addr(struct sockaddr * sa);             // get internet address
int get_server_socket(char *hostname, char *port);   // get a server socket
void print_ip( struct addrinfo *ai);                 // print IP info from getaddrinfo()

#endif  
