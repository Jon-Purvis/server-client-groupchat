# Default target: build both client and server
all: target/client target/server

# Rule to create the target directory if it doesn't exist
target:
	mkdir -p target

# Rule to build the client
target/client: client/client.c client/network.c client/auth.c target
	gcc -o target/client client/client.c client/network.c client/auth.c common/list.c -pthread

# Rule to build the server
target/server: server/server.c server/network.c server/auth.c target
	gcc -o target/server server/server.c server/network.c server/auth.c common/list.c -pthread -lcrypt

# Clean the build directory
clean:
	rm -f target/client target/server