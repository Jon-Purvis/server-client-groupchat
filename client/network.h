#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int get_server_connection(char *hostname, char *port);
void print_ip( struct addrinfo *ai);
void send_exit_command_to_server(int server_socket);
void send_generic_command_to_server(int server_socket, int command);

#endif
