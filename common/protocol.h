#ifndef PROTOCOL_H
#define PROTOCOL_H

// Communication protocol constants
#define BACKLOG 10

#define C2S_REGISTER 1       // Client-to-server registration request
#define C2S_LOGIN 2          // Client-to-server login request
#define C2S_JOIN_GROUP 3  
#define C2S_SEND_MESSAGE 4   // Sending message from client to server
#define C2S_LIST_ALL_MESSAGES 5 // Request for all messages
#define C2S_SEND_EXIT 6      // Client exit request

#define S2C_SENDING_ACK 1 
#define S2C_SENDING_MESSAGE 2
#define S2C_SENDING_ALL_MESSAGES 3

#define S2C_AUTHENTICATION_SUCCESS 100
#define S2C_AUTHENTICATION_FAILURE 109
#define S2C_JOIN_GROUP_SUCCESS 200
#define S2C_JOIN_GROUP_FAILURE 209
#define S2C_SEND_MESSAGE_SUCCESS 300
#define S2C_SEND_MESSAGE_FAILURE 309
#define S2C_GENERIC_SUCCESS 400
#define S2C_GENERIC_FAILURE 409

#endif
