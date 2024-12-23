#ifndef LIST_H 
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct USER { 
    char *email;
    char *password; 
    char *name; 
    int socket;   
    int isOnline;   
    struct USER *next; 
} User; 

typedef struct USER_LIST { 
    User *first;     
    User *last;      
    int count;      
} UserList; 

typedef struct GROUP_USER {
    User *user;
    struct GROUP_USER *next;
} GroupUser;

typedef struct GROUP_USER_LIST {
    GroupUser *first;
    GroupUser *last;
    int count;
} GroupUserList;

typedef struct GROUPS {
    GroupUserList *CMPS;
    GroupUserList *CMPS_352;
    GroupUserList *CMPS_340;
} Groups;

typedef struct MESSAGE { 
    char *message; 
    User *sender;      
    struct MESSAGE *next; 
} Message; 

typedef struct MESSAGE_LIST { 
    Message *first;   
    Message *last;   
    int count;         
} MessageList; 

typedef struct SESSION_DATA { 
    User *user;
    UserList *userList;
    Groups *groups;
} Session; 


void init_user_list(UserList *userList);
void init_group_user_list(GroupUserList *groupUserList);
void init_message_list(MessageList *messageList);

void append_user(UserList *userList, User *user);
void append_group_user(GroupUserList *groupUserList, User *user);
void append_message(MessageList *messageList, Message *message);

User *create_user(char *emailString, char *passwordString, char *nameString, int socket);
Message *create_message(char *messageString, User *senderString);
Groups *create_groups();
Session *create_session(User *user, UserList *userList, Groups *groups);

void free_user_list(UserList *userList);
void free_message_list(MessageList *messageList);
void free_groups(Groups *groups);
void free_user(User *user);

#endif  
