#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../common/list.h"  

int register_user(int server_socket);
int login_user(int server_socket);
void get_credentials(User *user, int isRegistration);
void send_credentials(int server_socket, User *user, int isRegistration);
int is_authentication_successful(int server_socket);

#endif 
