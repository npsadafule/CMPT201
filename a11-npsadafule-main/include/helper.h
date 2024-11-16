// helper.h
#pragma once

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Converts a buffer of bytes to a hexadecimal string representation
// Returns 0 on success, -1 on failure
int convert(uint8_t *buf,    // Input buffer containing bytes
            ssize_t buf_size, // Size of the input buffer
            char *str,        // Output string buffer (hex representation)
            ssize_t str_size  // Size of the output string buffer
);

// Define message types
#define MSG_TYPE_CHAT 0
#define MSG_TYPE_TERMINATE 1

// Structure for messages
struct message {
    uint8_t type;          // Message type (e.g., MSG_TYPE_CHAT)
    uint16_t length;       // Length of the message content
    char content[];        // Flexible array member for message content
};

// Functions to send and receive messages
ssize_t send_message(int sockfd, const struct message *msg);
ssize_t receive_message(int sockfd, struct message *msg, size_t buf_size);
