// client.h
#pragma once

#include <netinet/in.h>
#include <stddef.h>

// Starts the client
// Connects to the server at the specified IP and port
// Returns the connected socket file descriptor on success, -1 on failure
int start_client(const char *ip, const char *port);

// Sends data to the server
// Returns the number of bytes sent on success, -1 on failure
ssize_t client_send(int sockfd, const char *data, size_t data_size);

// Receives data from the server
// Returns the number of bytes received on success, -1 on failure
ssize_t client_receive(int sockfd, char *buffer, size_t buffer_size);

// Closes the client socket
void close_client(int sockfd);
