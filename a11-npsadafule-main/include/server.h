// server.h
#pragma once

#include <netinet/in.h>
#include <stddef.h>

// Maximum number of clients the server can handle
#define MAX_CLIENTS 10

// Starts the server
// Binds to the specified port and listens for incoming connections
// Returns the listening socket file descriptor on success, -1 on failure
int start_server(const char *port);

// Handles multiple clients using epoll
// Returns 0 on success, -1 on failure
int handle_clients(int listen_sock);

// Cleans up and closes the listening socket
void cleanup_server(int listen_sock);
