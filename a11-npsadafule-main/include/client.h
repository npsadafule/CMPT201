#pragma once

// Start client
// Connect to server at the IP and port specified
// Return connect socket file descriptor on success and -1 on fail
int start_client(const char *ip, const char *port);

// Close the client socket
void close_client(int sockfd);
