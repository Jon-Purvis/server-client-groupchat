#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"


void init_user_list(UserList *userList) {
   userList->first = NULL;
   userList->last = NULL;
   userList->count = 0;
}

void init_group_user_list(GroupUserList *groupUserList) {
   groupUserList->first = NULL;
   groupUserList->last = NULL;
   groupUserList->count = 0;
}

void init_message_list(MessageList *messageList) {
   messageList->first = NULL;
   messageList->last = NULL;
   messageList->count = 0;
}

void append_user(UserList *userList, User *user) {
   if (userList->first == NULL) {
      userList->first = user;
      userList->last = user;
   }
   else {
      userList->last->next = user;
      userList->last = user;
   }
   userList->count++;
}

void append_group_user(GroupUserList *groupUserList, User *user) {
   GroupUser *groupUser = (GroupUser *)malloc(sizeof(GroupUser));
   groupUser->user = user;
   groupUser->next = NULL;

   if (groupUserList->first == NULL) {
      groupUserList->first = groupUser;
      groupUserList->last = groupUser;
   }
   else {
      groupUserList->last->next = groupUser;
      groupUserList->last = groupUser;
   }
   groupUserList->count++;
}

void append_message(MessageList *messageList, Message *message) {
   if (messageList->first == NULL) {
      messageList->first = message;
      messageList->last = message;
   }
   else {
      messageList->last->next = message;
      messageList->last = message;
   }
   messageList->count++;
}

User *create_user(char *emailString, char *passwordString, char *nameString, int socket) {
   User *user = (User *) malloc(sizeof(User));
   user->email = emailString;
   user->password = passwordString;
   user->name = nameString;
   user->socket = socket;
   user->next = NULL;

   return user;
}

Message *create_message(char *messageString, User *senderString) {
   Message *message = (Message *) malloc(sizeof(Message));
   message->message = messageString;
   message->sender = senderString;
   message->next = NULL;

   return message;
}

Groups *create_groups() {
   Groups *groups = (Groups *)malloc(sizeof(Groups));

   groups->CMPS = (GroupUserList *)malloc(sizeof(GroupUserList));  
   groups->CMPS_352 = (GroupUserList *)malloc(sizeof(GroupUserList));
   groups->CMPS_340 = (GroupUserList *)malloc(sizeof(GroupUserList));

   init_group_user_list(groups->CMPS);
   init_group_user_list(groups->CMPS_352);
   init_group_user_list(groups->CMPS_340);

   return groups;
}

Session *create_session(User *user, UserList *userList, Groups *groups) {
    Session *session = (Session *) malloc(sizeof(Session));
    session->user = user;
    session->userList = userList;
    session->groups = groups;

    return session;
}

void free_user_list(UserList *userList) {
   User *current = userList->first;
   User *next;

   while (current != NULL) {
      next = current->next;    // Store next node
      free(current->email);    
      free(current->password); 
      free(current->name);     
      free(current);           // Free the user node
      current = next;          // Move to next node
   }
   free(userList);
}

void free_group_user_list(GroupUserList *groupUserList) {
   if (groupUserList == NULL) { 
      return;
   }

   GroupUser *current = groupUserList->first;
   GroupUser *next;

   while (current != NULL) {
      next = current->next;   // Store the next node
      free(current);          // Free the current node
      current = next;         // Move to the next node
   }

   free(groupUserList);       // Free the list structure itself
}

void free_message_list(MessageList *messageList) {
   Message *current = messageList->first;
   Message *next;

   while (current != NULL) {
      next = current->next;   // Store next node
      free(current->message); 
      free(current);         
      current = next;         // Move to next node
   }
   free(messageList);
}

void free_groups(Groups *groups) {
    if (groups) {
        free_group_user_list(groups->CMPS);
        free_group_user_list(groups->CMPS_352);
        free_group_user_list(groups->CMPS_340);
        free(groups);  
    }
}

void free_user(User *user) {
   free(user->email);    
   free(user->password); 
   free(user->name);
   free(user);
}

