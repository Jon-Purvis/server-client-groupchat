#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "network.h"
#include "../common/protocol.h"
#include "../common/list.h"
#include "auth.h"

void authentication_menu(int server_socket, int *authenticated);
void *main_menu(void *arg);
void print_menu_options(char *options[], int num_options);
int get_menu_choice();

void *listen_for_messages(void *arg);
void send_message(int socket);
void join_group(int server_socket);


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <hostname> <port>\n", argv[0]);
        return 1;
    }
    int server_socket = get_server_connection(argv[1], argv[2]);

    int isAuthenticated = 0;
    authentication_menu(server_socket, &isAuthenticated);
    if (isAuthenticated) {
        printf("Successful login\n");
        
        pthread_t receive_thread, send_thread;
        
        pthread_create(&receive_thread, NULL, listen_for_messages, (void *)(intptr_t)server_socket);
        pthread_create(&send_thread, NULL, main_menu, (void *)(intptr_t)server_socket);

        pthread_join(receive_thread, NULL);
        pthread_join(send_thread, NULL); 
    } else {
        printf("\nExiting client.\n");
    }

    close(server_socket);
    return 0;
}


void authentication_menu(int server_socket, int *authenticated) {
    int choice;

    char *auth_options[] = {"Register", "Login", "Exit"};
    int num_auth_options = 3;

    while (1) {
        print_menu_options(auth_options, num_auth_options);
        choice = get_menu_choice();

        switch (choice) {
            case 1: {
                if (register_user(server_socket)) {
                    *authenticated = 1;
                    return;
                }
                break;
            }
            case 2: {
                if (login_user(server_socket)) {
                    *authenticated = 1;
                    return;
                }
                break;
            }
            case 3: {
                send_exit_command_to_server(server_socket);
                return;
            }
            default: {
                printf("\nInvalid choice. Please try again.\n\n");
            }
        }
    }
}


void *main_menu(void *arg) {
    int server_socket = (intptr_t)arg;
    
    int choice;
    char *main_menu_options[] = {"Send Message to Group", "Join Group", "Exit"};
    int num_main_menu_options = 3;

    while (1) {
        print_menu_options(main_menu_options, num_main_menu_options);
        choice = get_menu_choice();

        switch (choice) {
            case 1: {
                send_message(server_socket);
                break;
            }
            case 2: {
                join_group(server_socket);
                break;
            }
            case 3: {
                send_exit_command_to_server(server_socket);
                pthread_exit(0);
            }
            default: {
                printf("\nInvalid choice. Please try again.\n\n");
            }
        }
    }
} 


void print_menu_options(char *options[], int num_options) {
    printf("\n--- Menu ---\n");
    for (int i = 0; i < num_options; i++) {
        printf("%d. %s\n", i + 1, options[i]);
    }
}


int get_menu_choice() {
    int choice;
    printf("Please choose an option: ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a valid number.\n");
        while (getchar() != '\n');
        // Return an invalid choice to prompt again
        return 0;
    }
    while (getchar() != '\n');  

    return choice;
}


void *listen_for_messages(void *arg) {
    int server_socket = (intptr_t)arg;
    char message[500];
    while (1) {
        int bytes_received = recv(server_socket, message, sizeof(message), 0);
        if (bytes_received <= 0) {
            break;  // Server disconnected
        }
        message[bytes_received] = '\0';  // Null-terminate the message
        printf("Received message: %s\n", message);
    }
    return NULL;
}


void send_message(int server_socket) {
    send_generic_command_to_server(server_socket, C2S_SEND_MESSAGE);

    char groupName[50];
    printf("Enter the name of the group you want to send the message to (CMPS, CMPS_340, or CMPS_352): ");
    fgets(groupName, sizeof(groupName), stdin);
    groupName[strcspn(groupName, "\n")] = 0;

    char *validGroups[] = {"CMPS", "CMPS_340", "CMPS_352"};
    int isValidGroup = 0;
    for (int i = 0; i < 3; i++) {
        if (strcmp(groupName, validGroups[i]) == 0) {
            isValidGroup = 1;
            break;
        }
    }
    if (!isValidGroup) {
        printf("Invalid group name. Please try again.\n");
        return;
    }

    int bytes_sent = send(server_socket, groupName, strlen(groupName), 0);
    if (bytes_sent == -1) {
        perror("Error sending group name to server");
        return;
    }

    char message[500];
    printf("Enter your message: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0;

    bytes_sent = send(server_socket, message, strlen(message), 0);
    if (bytes_sent == -1) {
        perror("Error sending message to server");
        return;
    }
    printf("Message sent to group %s.\n", groupName); 
}


void join_group(int server_socket) {
    char groupName[50];
    printf("Enter the name of the group you want to join (CMPS_340 or CMPS_352): ");
    fgets(groupName, sizeof(groupName), stdin);  
    groupName[strcspn(groupName, "\n")] = '\0';  

    send_generic_command_to_server(server_socket, C2S_JOIN_GROUP);

    int bytes_sent = send(server_socket, groupName, strlen(groupName), 0);
    if (bytes_sent == -1) {
        perror("Error sending group name to server");
        return;
    }
}
