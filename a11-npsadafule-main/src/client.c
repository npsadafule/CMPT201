// client.c
#include "client.h"
#include "helper.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

int start_client(const char *ip, const char *port) {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));

    // Convert IP address from presentation to network format
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    printf("Connected to server %s:%s\n", ip, port);
    return sockfd;
}

ssize_t client_send(int sockfd, const char *data, size_t data_size) {
    ssize_t sent = send(sockfd, data, data_size, 0);
    if (sent == -1) {
        perror("send");
    }
    return sent;
}

ssize_t client_receive(int sockfd, char *buffer, size_t buffer_size) {
    ssize_t received = recv(sockfd, buffer, buffer_size, 0);
    if (received == -1) {
        perror("recv");
    }
    return received;
}

void close_client(int sockfd) {
    close(sockfd);
    printf("Disconnected from server\n");
}

int main(int argc, char *argv[]) {
    printf("Debug: argc = %d\n", argc);

    const char *server_ip = "127.0.0.1"; // Default IP
    const char *server_port = "8000";    // Default port

    if (argc >= 2) {
        server_ip = argv[1]; // Use first argument as IP
    }
    if (argc >= 3) {
        server_port = argv[2]; // Use second argument as port
    }

    printf("Connecting to server at %s:%s\n", server_ip, server_port);

    int sockfd = start_client(server_ip, server_port);
    if (sockfd == -1) {
        exit(EXIT_FAILURE);
    }

    // Number of messages to send (for testing purposes)
    int num_messages = 10;

    // Buffer for receiving messages
    char recv_buf[BUF_SIZE];

    // Send random messages to the server
    for (int i = 0; i < num_messages; i++) {
        // Generate random data
        uint8_t rand_buf[10];
        char hex_str[10 * 2 + 1];
        if (getentropy(rand_buf, sizeof(rand_buf)) != 0) {
            perror("getentropy");
            break;
        }
        if (convert(rand_buf, sizeof(rand_buf), hex_str, sizeof(hex_str)) != 0) {
            fprintf(stderr, "Failed to convert random bytes to hex string\n");
            break;
        }

        // Send message to server
        struct message msg;
        msg.type = MSG_TYPE_CHAT;
        msg.length = strlen(hex_str);
        strcpy(msg.content, hex_str);

        if (send_message(sockfd, &msg) == -1) {
            fprintf(stderr, "Failed to send message\n");
            break;
        }

        printf("Sent message %d: %s\n", i + 1, hex_str);

        // Receive response from server (if necessary)
        // ssize_t received = client_receive(sockfd, recv_buf, BUF_SIZE - 1);
        // if (received > 0) {
        //     recv_buf[received] = '\0';
        //     printf("Received from server: %s\n", recv_buf);
        // }
    }

    // Close the connection
    close_client(sockfd);
    return 0;
}
