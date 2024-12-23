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
#include "../common/protocol.h"
#include "../common/list.h"


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


extern sem_t user_list_mutex;

void handle_authentication(Session *session, int *isAuthenticated) {

    int client_sock_fd = session->user->socket;
    int data_type = 0;

    while (1) {  
        if (recv(client_sock_fd, &data_type, sizeof(int), 0) <= 0) {
            printf("Client disconnected\n");
            return;
        }

        if (data_type == C2S_REGISTER) {
            if (handle_register(session)) {  
                *isAuthenticated = 1;
                return;  
            }
        } else if (data_type == C2S_LOGIN) {
            if (handle_login(session)) {  
                *isAuthenticated = 1;
                return;  
            }
        } else if (data_type == C2S_SEND_EXIT) {
            printf("Client requested exit\n");
            return; 
        } else {
            printf("Invalid data type received: %d\n", data_type);
            return;
        }
    }
}


int handle_register(Session *session) { 
    int client_sock_fd = session->user->socket;

    //sem_wait(&user_list_mutex);
    session->user = get_user_credentials(session, 1);
    //sem_post(&user_list_mutex);

    if (is_email_taken(session->user->email, session->userList)) {
        send_acknowledgement(client_sock_fd, S2C_AUTHENTICATION_FAILURE);
        return 0;
    } else {
        //sem_wait(&user_list_mutex);
        append_user(session->userList, session->user);
        append_group_user(session->groups->CMPS, session->user);
        session->user->isOnline = 1;
        //sem_post(&user_list_mutex);
        send_acknowledgement(client_sock_fd, S2C_AUTHENTICATION_SUCCESS);
        return 1;
    }
}


int handle_login(Session *session) {
    int client_sock_fd = session->user->socket;
    int data_type = 0;
    User *authenticatedUser = is_valid_login(session, session->userList);
    
    if (authenticatedUser) {
        // this authenticatedUser still has the socket from the last time they logged in.
        authenticatedUser->socket = client_sock_fd;        
        //sem_wait(&user_list_mutex);
        // update the socket of the user in the userList.
        session->user = authenticatedUser;
        session->user->isOnline = 1;
        //sem_post(&user_list_mutex);
        
        send_acknowledgement(client_sock_fd, S2C_AUTHENTICATION_SUCCESS);
        return 1;
    } else {
        send_acknowledgement(client_sock_fd, S2C_AUTHENTICATION_FAILURE);
    }
    return 0;
}


// Helper functions //
int is_email_taken(char *email, UserList *userList) {
    //sem_wait(&user_list_mutex);
    User *current = userList->first;
    while (current != NULL) {
        if (strcmp(current->email, email) == 0) {
            return 1; 
        }
        current = current->next;
    }
    //sem_post(&user_list_mutex);
    return 0; 
}


User *is_valid_login(Session *session, UserList *userList) {
    User *loginAttempt = get_user_credentials(session, 0);

    //sem_wait(&user_list_mutex);
    User *current = userList->first;
    while (current != NULL) {
        if (strcmp(current->email, loginAttempt->email) == 0) {
            if (authenticate(loginAttempt->password, current->password)) {
                free_user(loginAttempt);
                //sem_post(&user_list_mutex);
                return current;
            }
        }
        current = current->next;
    }
    free_user(loginAttempt);
    //sem_post(&user_list_mutex);
    // not a valid login
    return NULL;
}


User *get_user_credentials(Session *session, int isRegistration) {
    int client_sock_fd = session->user->socket;
    int expected_length = 0;

    recv(client_sock_fd, &expected_length, sizeof(int), 0);
    char *emailBuffer = (char *)malloc(expected_length);
    recv(client_sock_fd, emailBuffer, expected_length, 0);

    recv(client_sock_fd, &expected_length, sizeof(int), 0);
    char *passwordBuffer = (char *)malloc(expected_length);
    recv(client_sock_fd, passwordBuffer, expected_length, 0);

    // encrypt password
    char *encryptedPassword = encode(passwordBuffer);
    free(passwordBuffer);
    printf("ecnrypted password : %s\n", encryptedPassword);

    char *nameBuffer = NULL;
    if (isRegistration) {
        recv(client_sock_fd, &expected_length, sizeof(int), 0);
        nameBuffer = (char *)malloc(expected_length);
        recv(client_sock_fd, nameBuffer, expected_length, 0);
    }

    User *user = create_user(emailBuffer, encryptedPassword, isRegistration ? nameBuffer : session->user->name, client_sock_fd);
    return user;
}


// Encryption //
char* encode(char *plainpswd) {
   char *savedpswd;
   char salt[] = "$1$........";

   // set set encryption algorithm to SHA-256
   crypt_set_format("sha256");

   // encode the plainpassword
   generatesalt(salt);
   char *encoded = crypt(plainpswd, salt);

   // retrieve the password from the crypt buffer
   savedpswd = (char*) malloc(256);
   strcpy(savedpswd, encoded);

   return savedpswd;
}


int authenticate(char *loginpswd, char *savedpswd) {
   crypt_set_format("sha256");
   char* encodedloginpswd = crypt(loginpswd, savedpswd);
   return strcmp(encodedloginpswd, savedpswd) == 0;
}


void generatesalt(char salt[]) {
   unsigned long seed[2];

   const char *const seedchars =
      "./0123456789ABCDEFGHIJKLMNOPQRST"
      "UVWXYZabcdefghijklmnopqrstuvwxyz";

   seed[0] = time(NULL);
   seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);

   for (int i = 0; i < 8; i++)
      salt[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];

   return;
}
