#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include "network.h"
#include "../common/list.h"
#include "../common/protocol.h"

void handle_authentication(Session *session, int *isAuthenticated);
int handle_register(Session *session);
int handle_login(Session *session);

// Helper functions //
int is_email_taken(char *email, UserList *userList);
User *is_valid_login(Session *session, UserList *userList);
User *get_user_credentials(Session *session, int isRegistration);

// Encryption functions //
char* encode(char *plainpswd);
int authenticate(char *loginpswd, char *savedpswd);
void generatesalt(char salt[]);

#endif
