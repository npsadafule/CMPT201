// server.c
#include "server.h"
#include "helper.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define MAX_EVENTS 10

int start_server(const char *port) {
    int listen_sock;
    struct sockaddr_in server_addr;
    int opt = 1;

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == -1) {
        perror("socket");
        return -1;
    }

    // Set socket options to reuse address and port
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(listen_sock);
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces
    server_addr.sin_port = htons(atoi(port));

    if (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(listen_sock);
        return -1;
    }

    if (listen(listen_sock, MAX_CLIENTS) == -1) {
        perror("listen");
        close(listen_sock);
        return -1;
    }

    printf("Server is listening on port %s\n", port);

    return listen_sock;
}

int handle_clients(int listen_sock) {
    int epollfd;
    struct epoll_event ev, events[MAX_EVENTS];
    int nfds;

    // Create epoll instance
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        return -1;
    }

    // Add the listening socket to epoll
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        close(epollfd);
        return -1;
    }

    // Buffer for incoming messages
    char buf[BUF_SIZE];

    while (1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            close(epollfd);
            return -1;
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                // Accept new connection
                int conn_sock;
                struct sockaddr_in client_addr;
                socklen_t addrlen = sizeof(client_addr);
                conn_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addrlen);
                if (conn_sock == -1) {
                    perror("accept");
                    continue;
                }

                // Set non-blocking
                int flags = fcntl(conn_sock, F_GETFL, 0);
                if (flags == -1) flags = 0;
                fcntl(conn_sock, F_SETFL, flags | O_NONBLOCK);

                // Add new socket to epoll
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    close(conn_sock);
                    continue;
                }

                printf("Accepted connection from %s:%d\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));

            } else {
                // Handle client data
                int client_fd = events[n].data.fd;
                ssize_t count = read(client_fd, buf, sizeof(buf));
                if (count == -1) {
                    if (errno != EAGAIN) {
                        perror("read");
                        close(client_fd);
                    }
                } else if (count == 0) {
                    // Connection closed
                    printf("Client disconnected\n");
                    close(client_fd);
                } else {
                    // Process data
                    buf[count] = '\0';
                    printf("Received: %s\n", buf);
                    // Echo back to client
                    write(client_fd, buf, count);
                }
            }
        }
    }

    close(epollfd);
    return 0;
}

void cleanup_server(int listen_sock) {
    close(listen_sock);
    printf("Server shutdown\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./server <port>\n");
        exit(EXIT_FAILURE);
    }

    int listen_sock = start_server(argv[1]);
    if (listen_sock == -1) {
        exit(EXIT_FAILURE);
    }

    if (handle_clients(listen_sock) == -1) {
        cleanup_server(listen_sock);
        exit(EXIT_FAILURE);
    }

    cleanup_server(listen_sock);
    return 0;
}
