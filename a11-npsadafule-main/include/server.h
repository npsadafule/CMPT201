#pragma once

// Start the server
// Bind to the port specified and listen for incoming connections
// It returns the listening socket file descriptor on success and -1 on failure
int start_server(const char *port, int expected_clients);

// It cleans up and closes the listening socket
void cleanup_server(int listen_socket);
