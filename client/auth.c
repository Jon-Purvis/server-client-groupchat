#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "auth.h"
#include "../common/list.h"
#include "../common/protocol.h"


int register_user(int server_socket);
int login_user(int server_socket);

// Helper functions //
void get_credentials(User *user, int isRegistration);
void send_credentials(int server_socket, User *user, int isRegistration);
int is_authentication_successful(int server_socket);


int register_user(int server_socket) {
    User *user = malloc(sizeof(User));
    while (1) {
        get_credentials(user, 1);
        send_credentials(server_socket, user, 1);

        if (is_authentication_successful(server_socket)) {
            return 1; 
        } else {
            printf("You are already registered! Please login instead.\n");
            free_user(user);
            return 0;
        }
    }
}


int login_user(int server_socket) {
    User *user = malloc(sizeof(User));
    while (1) {
        get_credentials(user, 0);
        send_credentials(server_socket, user, 0);

        if (is_authentication_successful(server_socket)) {
            return 1;
        } else {
            printf("Wrong username or password! Please try again.\n");
            free_user(user);
            return 0;
        }
    }
}


void get_credentials(User *user, int isRegistration) {
    size_t size = 0;
    char *input = NULL;

    // Get email
    printf("Enter your email: ");
    getline(&input, &size, stdin);
    input[strcspn(input, "\n")] = 0;
    user->email = strdup(input);

    // Get password
    printf("Enter your password: ");
    getline(&input, &size, stdin);
    input[strcspn(input, "\n")] = 0;
    user->password = strdup(input);

    if (isRegistration) {
        printf("Enter your name: ");
        getline(&input, &size, stdin);
        input[strcspn(input, "\n")] = 0;
        user->name = strdup(input);
    }

    free(input);
}


void send_credentials(int server_socket, User *user, int isRegistration) {
    int type = C2S_LOGIN;
    if (isRegistration) { type = C2S_REGISTER; }
    
    send(server_socket, &type, sizeof(int), 0);

    // Send email
    int emailLength = strlen(user->email) + 1;
    send(server_socket, &emailLength, sizeof(int), 0);
    send(server_socket, user->email, emailLength, 0);

    // Send password
    int passwordLength = strlen(user->password) + 1;
    send(server_socket, &passwordLength, sizeof(int), 0);
    send(server_socket, user->password, passwordLength, 0);

    // registration needs name as well
    if (isRegistration) {
        int nameLength = strlen(user->name) + 1;
        send(server_socket, &nameLength, sizeof(int), 0);
        send(server_socket, user->name, nameLength, 0);
    }
}


int is_authentication_successful(int server_socket) {
    int acknowledgment;
    recv(server_socket, &acknowledgment, sizeof(int), 0);

    if (acknowledgment == S2C_AUTHENTICATION_SUCCESS) {
        printf("Operation successful!\n\n");
        return 1;
    } else if (acknowledgment == S2C_AUTHENTICATION_FAILURE) {
        printf("Operation failed.\n\n");
        return 0;
    } else {
        printf("Invalid request: %d\n\n", acknowledgment);
        return 0;
    }
}


