// helper.c
#include "helper.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int convert(uint8_t *buf, ssize_t buf_size, char *str, ssize_t str_size) {
    if (buf == NULL || str == NULL || buf_size <= 0 ||
        str_size < (buf_size * 2 + 1)) {
        return -1;
    }

    for (int i = 0; i < buf_size; i++) {
        sprintf(str + i * 2, "%02X", buf[i]);
    }
    str[buf_size * 2] = '\0';

    return 0;
}

ssize_t send_message(int sockfd, const struct message *msg) {
    uint16_t net_length = htons(msg->length);
    // Send message type
    if (send(sockfd, &msg->type, sizeof(msg->type), 0) != sizeof(msg->type))
        return -1;
    // Send message length
    if (send(sockfd, &net_length, sizeof(net_length), 0) != sizeof(net_length))
        return -1;
    // Send message content
    if (send(sockfd, msg->content, msg->length, 0) != msg->length)
        return -1;
    return sizeof(msg->type) + sizeof(net_length) + msg->length;
}

ssize_t receive_message(int sockfd, struct message *msg, size_t buf_size) {
    uint16_t net_length;
    // Receive message type
    if (recv(sockfd, &msg->type, sizeof(msg->type), 0) != sizeof(msg->type))
        return -1;
    // Receive message length
    if (recv(sockfd, &net_length, sizeof(net_length), 0) != sizeof(net_length))
        return -1;
    msg->length = ntohs(net_length);
    if (msg->length > buf_size)
        return -1; // Buffer too small
    // Receive message content
    if (recv(sockfd, msg->content, msg->length, 0) != msg->length)
        return -1;
    return sizeof(msg->type) + sizeof(net_length) + msg->length;
}
