#include "client.h"
#include "helper.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_MESSAGE_SIZE 1024

typedef struct {
  int sockfd;
  FILE *log_file;
} ReceiverArgs;

void *receive_messages(void *args) {
  ReceiverArgs *receiver_args = (ReceiverArgs *)args;
  int sockfd = receiver_args->sockfd;
  FILE *log_file = receiver_args->log_file;
  uint8_t buffer[MAX_MESSAGE_SIZE];
  ssize_t bytes_received;

  while (1) {
    // Read messsage type
    bytes_received = recv(sockfd, buffer, 1, 0);
    if (bytes_received <= 0) {
      perror("Error receiving message type");
      break;
    }

    uint8_t type = buffer[0];

    if (type == MSG_TYPE_CHAT) {
      // Receive IP (4 bytes) and port (2 bytes)
      bytes_received = recv(sockfd, buffer, 6, MSG_WAITALL);
      if (bytes_received != 6) {
        perror("Error receiving IP and port");
        break;
      }

      uint32_t sender_ip;
      uint16_t sender_port;
      memcpy(&sender_ip, buffer, 4);
      memcpy(&sender_port, buffer + 4, 2);

      // Convert IP and port to host byte order
      sender_ip = ntohl(sender_ip);
      sender_port = ntohs(sender_port);

      // Convert IP to string
      struct in_addr ip_addr;
      ip_addr.s_addr = htonl(sender_ip);
      char ip_str[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &ip_addr, ip_str, INET_ADDRSTRLEN);

      // Receive the message until \n
      char message[MAX_MESSAGE_SIZE];
      ssize_t total_received = 0;
      while (total_received < MAX_MESSAGE_SIZE - 1) {
        bytes_received = recv(sockfd, buffer, 1, 0);
        if (bytes_received <= 0) {
          perror("Error while receiving message");
          break;
        }
        message[total_received++] = buffer[0];
        if (buffer[0] == '\n') {
          break;
        }
      }
      message[total_received - 1] = '\0'; // removing the newline character

      // Logging the message
      fprintf(log_file, "%-15s%-10u%s\n", ip_str, sender_port, message);
      fflush(log_file);
      // Printing to stdout for debbuging
      // printf("%-15s%-10u%s\n". ip_str, sender_port, message);
    } else if (type == MSG_TYPE_TERMINATE) {
      // server terminating
      printf("Receiving termination msg from server\n");
      break;
    } else {
      fprintf(stderr, "Message type unknown: %u\n", type);
      break;
    }
  }
  pthread_exit(NULL);
}

int start_client(const char *ip, const char *port) {
  int sockfd;
  struct sockaddr_in server_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("Error in socket");
    return -1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(port));

  // Convert IP address to the correct format
  if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
    perror("Error in inet_pton");
    close(sockfd);
    return -1;
  }

  // Connecting to server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("Error in connect");
    close(sockfd);
    return -1;
  }

  printf("Connect to server %s:%s\n", ip, port);
  return sockfd;
}

void close_client(int sockfd) {
  close(sockfd);
  printf("Disconnected from server\n");
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr, "Format: ./client <IP address> <port number> <# of "
                    "messages> <log file path>\n");
    exit(EXIT_FAILURE);
  }

  const char *server_ip = argv[1];
  const char *server_port = argv[2];
  int num_messages = atoi(argv[3]);
  const char *log_file_path = argv[4];

  int sockfd = start_client(server_ip, server_port);
  if (sockfd == -1) {
    exit(EXIT_FAILURE);
  }

  // open log file
  FILE *log_file = fopen(log_file_path, "w");
  if (!log_file) {
    perror("Error opening log file");
    close_client(sockfd);
    exit(EXIT_FAILURE);
  }

  // Starting receiver thread
  pthread_t receiver_thread;
  ReceiverArgs receiver_args = {sockfd, log_file};

  if (pthread_create(&receiver_thread, NULL, receive_messages,
                     &receiver_args) != 0) {
    perror("Error creating receiver thread");
    fclose(log_file);
    close_client(sockfd);
    exit(EXIT_FAILURE);
  }

  // Sending random messages to server
  for (int i = 0; i < num_messages; i++) {
    // generating random data
    uint8_t rand_buff[10];
    char hex_str[10 * 2 + 1];
    if (getentropy(rand_buff, sizeof(rand_buff)) != 0) {
      perror("Error in getentropy");
      break;
    }

    if (convert(rand_buff, sizeof(rand_buff), hex_str, sizeof(hex_str)) != 0) {
      fprintf(stderr, "Failed to convert random bytes to hex string\n");
      break;
    }

    // constructing the message
    uint8_t message[MAX_MESSAGE_SIZE];
    size_t message_length = 0;

    // message type
    message[message_length++] = MSG_TYPE_CHAT;

    // message content
    size_t content_length = strlen(hex_str);
    memcpy(message + message_length, hex_str, content_length);
    message_length += content_length;

    // adding newline
    message[message_length++] = '\n';

    // sending message to server
    if (send(sockfd, message, message_length, 0) != (ssize_t)message_length) {
      perror("Error sending message");
      break;
    }

    // print to stdout for debugging
    // printf("Sent message %d: %s\n", i+1, hex_str);
  }

  // Send termination message
  uint8_t terminate_message[2];
  terminate_message[0] = MSG_TYPE_TERMINATE;
  terminate_message[1] = '\n';
  if (send(sockfd, terminate_message, 2, 0) != 2) {
    perror("Error in sending termination message");
  }

  // waiting for reciver tread to finish
  pthread_join(receiver_thread, NULL);

  fclose(log_file);
  close_client(sockfd);
  return 0;
}
