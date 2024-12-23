# TCP Group Chat Application

## Overview

This project implements a TCP-based group chat system with client-server architecture in C. The application supports multiple clients, user authentication, group management, and secure messaging. Users can join groups and send messages, which are only delivered to online members of the intended group.

---

## Features

- **User Authentication**: Supports login and registration.
- **Group Chat**: Users can create or join groups and send messages to group members.
- **Message Delivery**: Messages are delivered only to members of the group who are online.
- **Cross-Platform Deployment**: The application can run locally or on AWS using Docker.

---

## Project Structure

```plaintext
.
├── client/
│   ├── auth.c            # Handles client-side authentication logic
│   ├── auth.h            # Header for authentication module
│   ├── client.c          # Main client application logic
│   ├── network.c         # Handles network communication for client
│   └── network.h         # Header for client network module
├── common/
│   ├── list.c            # Utility functions for managing lists
│   ├── list.h            # Header for list utility
│   └── protocol.h        # Common protocol definitions
├── compose.yaml          # Docker Compose configuration
├── Dockerfile            # Dockerfile for building containers
├── Makefile              # Makefile for compiling the project
├── server/
│   ├── auth.c            # Handles server-side authentication logic
│   ├── auth.h            # Header for server authentication module
│   ├── network.c         # Handles network communication for server
│   ├── network.h         # Header for server network module
│   └── server.c          # Main server application logic
└── target/               # Output directory for compiled binaries
    └── client            # Client executable
```

---

## Prerequisites

- **Docker** installation
- Basic knowledge of TCP networking
- Access to an AWS server (if deploying remotely)

---

## Setup Instructions

### Running Locally

#### **1. Start the Server**
Run the following command to start the server:
```bash
docker compose run --rm --name server server
```

#### **2. Start the Client**
Start a client instance with:
```bash
docker compose run --rm --name client1 client
```
> Replace \`client1\` with unique names like \`client2\`, \`client3\` for additional clients.

#### **3. Usage**
- Server: After the container starts, run the compiled code:
  ```bash
  ./server <server_ip> <port_number>
  ```
- Client: Connect using the compiled client:
  ```bash
  ./client <server_ip> <port_number>
  ```

---

### Running on AWS

#### **1. Transfer Files**
Transfer the project files to your AWS server:
```bash
scp -r ./server ./client ./common Makefile firstname-lastname@ip:/usr/home/firstname-lastname/
```

#### **2. SSH into AWS**
Connect to the AWS server:
```bash
ssh firstname-lastname@ip
```

#### **3. Compile the Project**
Manually clean, build, and navigate to the \`target\` directory:
```bash
make clean
make
cd target
```

#### **4. Start the Server**
Run the server:
```bash
./server <server_ip> <port_number>
```

#### **5. Start a Remote Client**
Start a remote client with Docker Compose:
```bash
docker compose run --rm --name remoteclient1 remoteclient
```
> Replace \`remoteclient1\` with unique names like \`remoteclient2\`, \`remoteclient3\` for additional clients.

Clients can connect using:
```bash
./client <server_ip> <port_number>
```

---

## Authentication and Group Chat Usage

1. **Login/Register**: Users must first authenticate using login or registration.
2. **Join Groups**: Clients can join groups or create new ones.
3. **Send Messages**: Send messages to groups. Only online members of the group will receive the message.

---

## Notes

- Ensure you run the server prior to the client program.
- Docker Compose automates the compilation process for local execution.
- For AWS deployment, the manual compilation and navigation steps are required before running executables.

---


## Future Improvements

- Add message history persistence.
- Improve user experience with a CLI interface.

---

## Contributors

- **Jonathon Purvis** 
- **Emily Alexa Gotiango**
- **Grace Lloyd**

---

