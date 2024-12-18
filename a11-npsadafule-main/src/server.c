#include "server.h"
#include "helper.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 100       // Given in instructions
#define MAX_MESSAGE_SIZE 1024 // given in instructions

typedef struct {
  int sockfd;
  struct sockaddr_in addr;
} Client;

typedef struct Message {
  uint8_t data[MAX_MESSAGE_SIZE];
  size_t length;
  struct Message *next;
} Message;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int expected_clients = 0;
int termination_count = 0;

Message *message_queue_head = NULL;
Message *message_queue_tail = NULL;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

// for synchronization during termination
int termination_message_sent = 0;
pthread_mutex_t termination_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t termination_cond = PTHREAD_COND_INITIALIZER;

void *broadcast_message(void *arg) {
  while (1) {
    pthread_mutex_lock(&queue_mutex);
    while (message_queue_head == NULL) {
      pthread_cond_wait(&queue_cond, &queue_mutex);
    }

    Message *node = message_queue_head;
    message_queue_head = node->next;
    if (message_queue_head == NULL) {
      message_queue_tail = NULL;
    }
    pthread_mutex_unlock(&queue_mutex);

    // Broadcasting message to all clients
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
      if (send(clients[i].sockfd, node->data, node->length, 0) !=
          (ssize_t)node->length) {
        perror("Error broadcasting message");
      }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (node->data[0] == MSG_TYPE_TERMINATE) {
      // signal termination message sent
      pthread_mutex_lock(&termination_mutex);
      termination_message_sent = 1;
      pthread_cond_broadcast(&termination_cond);
      pthread_mutex_unlock(&termination_mutex);

      // Clean up and exit
      free(node);
      break;
    }

    free(node);
  }

  pthread_exit(NULL);
}

void queue_message(uint8_t *message, size_t length) {
  Message *node = malloc(sizeof(Message));
  if (!node) {
    perror("Memory allocation failed for Message");
    exit(EXIT_FAILURE);
  }
  memcpy(node->data, message, length);
  node->length = length;
  node->next = NULL;

  pthread_mutex_lock(&queue_mutex);
  if (message_queue_tail == NULL) {
    message_queue_head = message_queue_tail = node;
  } else {
    message_queue_tail->next = node;
    message_queue_tail = node;
  }
  pthread_cond_signal(&queue_cond);
  pthread_mutex_unlock(&queue_mutex);
}

void *handle_client(void *arg) {
  Client client = *(Client *)arg;
  free(arg); // freeing memory allocted for client info
  uint8_t buffer[MAX_MESSAGE_SIZE];
  ssize_t bytes_received;
  int client_sent_termination = 0; // flag to cleck if client sent termination

  while (1) {
    // read the message type
    bytes_received = recv(client.sockfd, buffer, 1, MSG_WAITALL);
    if (bytes_received <= 0) {
      if (bytes_received == 0) {
        // client disconnected
      } else {
        perror("Error receiving message type");
      }
      goto disconnect;
    }

    uint8_t type = buffer[0];

    if (type == MSG_TYPE_CHAT) {
      // have to receive untill \n
      size_t total_received = 0;
      while (total_received < MAX_MESSAGE_SIZE - 2) {
        bytes_received = recv(client.sockfd, buffer + 1 + total_received, 1, 0);
        if (bytes_received <= 0) {
          if (bytes_received == 0) {
            printf("Client disconnected\n");
          } else {
            perror("error in receiving message");
          }
          goto disconnect;
        }
        if (buffer[1 + total_received] == '\n') {
          total_received++;
          break;
        }
        total_received += bytes_received;
      }

      // preping the message to add to queue
      uint8_t message[MAX_MESSAGE_SIZE];
      size_t message_length = 0;

      // message type
      message[message_length++] = MSG_TYPE_CHAT;

      // sender IP and port
      uint32_t sender_ip = client.addr.sin_addr.s_addr;
      uint16_t sender_port = client.addr.sin_port;
      memcpy(message + message_length, &sender_ip, 4);
      message_length += 4;
      memcpy(message + message_length, &sender_port, 2);
      message_length += 2;

      // message
      memcpy(message + message_length, buffer + 1, total_received);
      message_length += total_received;

      // adding message to queue
      queue_message(message, message_length);
    } else if (type == MSG_TYPE_TERMINATE) {
      client_sent_termination = 1; // client sent termination message
      pthread_mutex_lock(&clients_mutex);
      termination_count++;
      if (termination_count == expected_clients) {
        // all clients have sent termination message, queing termination
        // message;
        uint8_t terminate_message[2];
        terminate_message[0] = MSG_TYPE_TERMINATE;
        terminate_message[1] = '\n';
        queue_message(terminate_message, 2);
      }
      pthread_mutex_unlock(&clients_mutex);

      // wait for termination message
      pthread_mutex_lock(&termination_mutex);
      while (!termination_message_sent) {
        pthread_cond_wait(&termination_cond, &termination_mutex);
      }
      pthread_mutex_unlock(&termination_mutex);
      break;
    } else {
      fprintf(stderr, "Message type unknown from client: %u\n", type);
      goto disconnect;
    }
  }

disconnect:
  pthread_mutex_lock(&clients_mutex);
  if (!client_sent_termination) {
    // client disconnected unexpectedly
    termination_count++;
    if (termination_count == expected_clients) {
      // all clients have terminated or disconnected
      uint8_t terminate_message[2];
      terminate_message[0] = MSG_TYPE_TERMINATE;
      terminate_message[1] = '\n';
      queue_message(terminate_message, 2);
    }
  }
  // remove client from client array and close socket
  for (int i = 0; i < client_count; i++) {
    if (clients[i].sockfd == client.sockfd) {
      // close client socket
      close(clients[i].sockfd);

      // shifting the array to remove the client
      for (int j = i; j < client_count - 1; j++) {
        clients[j] = clients[j + 1];
      }
      client_count--;
      break;
    }
  }
  pthread_mutex_unlock(&clients_mutex);
  pthread_exit(NULL);
}

int start_server(const char *port_str, int expected_clients_param) {
  expected_clients = expected_clients_param;

  int listen_socket;
  struct sockaddr_in server_address;

  listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket == -1) {
    perror("Error in socket");
    return -1;
  }

  int opt = 1;
  if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("error in setsockopt");
    close(listen_socket);
    return -1;
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY; // binding to all interface
  server_address.sin_port = htons(atoi(port_str));

  if (bind(listen_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    perror("error in binding");
    close(listen_socket);
    return -1;
  }

  if (listen(listen_socket, MAX_CLIENTS) == -1) {
    perror("error in listening");
    close(listen_socket);
    return -1;
  }

  printf("Server listening on port %s\n", port_str);

  // start the broadcasting thread
  pthread_t broadcast_thread;
  if (pthread_create(&broadcast_thread, NULL, broadcast_message, NULL) != 0) {
    perror("error in creating broadcaster thread");
    close(listen_socket);
    return -1;
  }

  while (1) {
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int client_sockfd =
        accept(listen_socket, (struct sockaddr *)&client_address, &client_len);
    if (client_sockfd == -1) {
      perror("error in accepting");
      continue;
    }

    // adding client to list
    pthread_mutex_lock(&clients_mutex);
    clients[client_count].sockfd = client_sockfd;
    clients[client_count].addr = client_address;
    client_count++;
    pthread_mutex_unlock(&clients_mutex);

    // creating thread to handle client
    pthread_t client_thread;
    Client *client_info = malloc(sizeof(Client));
    if (!client_info) {
      perror("memory allocation failed for client info");
      close(client_sockfd);
      continue;
    }
    *client_info = clients[client_count - 1];

    if (pthread_create(&client_thread, NULL, handle_client, client_info) != 0) {
      perror("error in creating client thread");
      close(client_sockfd);
      free(client_info);
      continue;
    }

    pthread_detach(client_thread);
  }

  // wating for brodcaster thread to finish
  pthread_join(broadcast_thread, NULL);
  return listen_socket;
}

void cleanup_server(int listen_socket) {
  close(listen_socket);
  printf("Server shut down");
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Format: ./server <port number> <# of clients>\n");
    exit(EXIT_FAILURE);
  }

  const char *port = argv[1];
  int clients_expected = atoi(argv[2]);

  int listen_socket = start_server(port, clients_expected);
  if (listen_socket == -1) {
    exit(EXIT_FAILURE);
  }

  cleanup_server(listen_socket);
  return 0;
}
