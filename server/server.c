/*
 * Author: Jon, Emily, and Grace
 * Date: 12/5/2024
 *
 * Purpose:
 * A simple multi-client TCP server that listens for connections, 
 * processes client messages, and supports basic messaging functionality. 
 * Each client session is handled in a separate thread.
 *
 * Compilation Instructions:
 * To compile the program, use the following command in a terminal:
 * 
 *     gcc -o server server.c server-helper.c list.c -pthread  
 *     or simply: make 
 *
 * Running Instructions:
 * To run the server, use the following command:
 * 
 *     ./server <host> <port>
 * 
 * Examples:
 *     Docker: 
 *     ./server server 33003
 *
 *     AWS:
 *     ./server 172.30.129.135 33003
 *
 * Note: Ensure you run the server program prior to running the client program.
 */

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
#include "auth.h"


void *handle_client(void *ptr);
void handle_message(Session *session);
void broadcast_message(char *groupName, char *message, Session *sender);
void handle_join_group(Session *session);

// Helper function //
void print_group_members(char *group_name, Groups *groups);
GroupUserList *get_group_by_name(Groups *groups, char *groupName);
int is_user_in_group(GroupUserList *groupUserList, User *user);


sem_t user_list_mutex;

int main(int argc, char *argv[]) {
    int sock_fd;
    int client_sock_fd;

    // initialize mutex
    int pshared = 0;
    int success;
    int value = 1;
    success = sem_init(&user_list_mutex, pshared, value);
    if (success != 0) {
        perror("Semaphore initialization failed");
        exit(1);
    }

    sock_fd = start_server(argv[1], argv[2], BACKLOG);
    if (sock_fd == -1) {
        perror("Failed to start server");
        exit(1);
    }

    Groups *groups = create_groups();
    UserList *userList = (UserList *)malloc(sizeof(UserList));
    init_user_list(userList);

    while (1) {
        if ((client_sock_fd = accept_client(sock_fd)) == -1) {
            perror("Failed to accept client");
            continue;
        }

        User *user = create_user(NULL, NULL, NULL, client_sock_fd);
        Session *session = create_session(user, userList, groups);

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void *)session);
    }
}


void *handle_client(void *ptr) {
    Session *session = (Session *)ptr;
    
    int isAuthenticated = 0; 
    handle_authentication(session, &isAuthenticated);

    if (isAuthenticated) {
        printf("Welcome, %s\n", session->user->name);
        print_group_members("CMPS", session->groups);
        print_group_members("CMPS_340", session->groups);
        print_group_members("CMPS_352", session->groups);

        int data_type = 0;
        recv(session->user->socket, &data_type, sizeof(int), 0);
        while (data_type != C2S_SEND_EXIT) {
            if (data_type == C2S_SEND_MESSAGE) {
                handle_message(session);
            }
            else if (data_type == C2S_JOIN_GROUP) {
                handle_join_group(session);
                print_group_members("CMPS", session->groups);
                print_group_members("CMPS_340", session->groups);
                print_group_members("CMPS_352", session->groups);

            }
            else { 
                printf("Invalid data type\n"); 
            }
            recv(session->user->socket, &data_type, sizeof(int), 0);
        }
        disconnect_client(session);
    }
    return NULL; 
}


void handle_message(Session *session) {
    char groupName[50];
    int bytes_received;

    bytes_received = recv(session->user->socket, groupName, sizeof(groupName), 0);
    if (bytes_received <= 0) {
        return;  
    }
    groupName[bytes_received] = '\0';  

    char message[500];
    bytes_received = recv(session->user->socket, message, sizeof(message), 0);
    if (bytes_received <= 0) {
        return;  
    }
    message[bytes_received] = '\0';  
    printf("Received from %s for group %s: %s\n", session->user->name, groupName, message);

    // show message to specific group
    broadcast_message(groupName, message, session);
}


void broadcast_message(char *groupName, char *message, Session *session) {
    GroupUserList *targetGroup = NULL;

    // Determine the target group based on the groupName
    if (strcmp(groupName, "CMPS") == 0) {
        targetGroup = session->groups->CMPS;
    } else if (strcmp(groupName, "CMPS_352") == 0) {
        targetGroup = session->groups->CMPS_352;
    } else if (strcmp(groupName, "CMPS_340") == 0) {
        targetGroup = session->groups->CMPS_340;
    }

    if (targetGroup != NULL) {
        // Iterate through the users in the group and broadcast the message
        for (GroupUser *groupUser = targetGroup->first; groupUser != NULL; groupUser = groupUser->next) {
            // Don't send the message to the sender
            if (groupUser->user != session->user) {
                // Make sure the user is online
                if (groupUser->user->isOnline) {
                    send(groupUser->user->socket, message, strlen(message), 0);
                }
            }
        }
    }
}


void handle_join_group(Session *session) {
    char groupName[50];
    int bytesReceived = recv(session->user->socket, groupName, sizeof(groupName), 0);
    if (bytesReceived <= 0) {
        return;  
    }
    groupName[bytesReceived] = '\0';  

    if (strcmp(groupName, "CMPS_340") == 0) { 
        if (!is_user_in_group(session->groups->CMPS_340, session->user)) {
            append_group_user(session->groups->CMPS_340, session->user);
            printf("after insert 340 group: \n");
            print_group_members("CMPS", session->groups);
            print_group_members("CMPS_340", session->groups);
            print_group_members("CMPS_352", session->groups);
        } else {
            printf("already in group\n");
        }
    }
    if (strcmp(groupName, "CMPS_352") == 0) { 
        if (!is_user_in_group(session->groups->CMPS_352, session->user)) {
            append_group_user(session->groups->CMPS_352, session->user);
            printf("after insert 352 group: \n");
            print_group_members("CMPS", session->groups);
            print_group_members("CMPS_340", session->groups);
            print_group_members("CMPS_352", session->groups);
        } else {
            printf("already in group\n");
        }
    }

    printf("Successfully joined group\n");
}


GroupUserList *get_group_by_name(Groups *groups, char *groupName) {
    if (strcmp(groupName, "CMPS") == 0) { return groups->CMPS; }
    if (strcmp(groupName, "CMPS_340") == 0) { return groups->CMPS_340; }
    if (strcmp(groupName, "CMPS_352") == 0) { return groups->CMPS_352; }
    return NULL;
}


int is_user_in_group(GroupUserList *groupUserList, User *user) {
    GroupUser *current = groupUserList->first;
    while (current != NULL) {
        if (current->user == user) {
            return 1;  
        }
        current = current->next;
    }
    return 0;  
}


// Function to print all members of a specified group
void print_group_members(char *group_name, Groups *groups) {
    GroupUserList *target_group = get_group_by_name(groups, group_name);
    
    if (target_group == NULL) {
        printf("Group '%s' has no members.\n", group_name);
        return;
    }

    printf("Members of group '%s':\n", group_name);

    GroupUser *current = target_group->first;
    while (current != NULL) {
        printf(" - Name: %s, Email: %s, Socket: %d\n", current->user->name, current->user->email, current->user->socket);
        current = current->next;
    }
}




