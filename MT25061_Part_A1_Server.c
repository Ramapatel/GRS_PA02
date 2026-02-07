// Roll No: MT25061
// PA02 – Part A1 Server (Two-Copy Implementation)
// Updated ONLY to support graceful shutdown for server-side perf

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#define SERVER_PORT 8080
#define BACKLOG 16

// --------------------------------------------------
// Global stop flag (used for graceful shutdown)
// --------------------------------------------------
static volatile sig_atomic_t stop_server = 0;

// --------------------------------------------------
// Signal handler
// --------------------------------------------------
void handle_signal(int sig) {
    stop_server = 1;
}

// --------------------------------------------------
// Worker thread: sends fixed-size messages repeatedly
// --------------------------------------------------
void *client_handler(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    size_t msg_size = 0;
    recv(client_fd, &msg_size, sizeof(msg_size), 0);

    char *buffer = malloc(msg_size);
    if (!buffer) {
        close(client_fd);
        return NULL;
    }

    memset(buffer, 'A', msg_size);

    // Send data until server is asked to stop
    while (!stop_server) {
        ssize_t sent = send(client_fd, buffer, msg_size, 0);
        if (sent <= 0)
            break;
    }

    free(buffer);
    close(client_fd);
    return NULL;
}

// --------------------------------------------------
// Main
// --------------------------------------------------
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <message_size> <threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Register signal handlers for clean termination
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // --------------------------------------------------
    // Accept loop (exits cleanly on SIGTERM)
    // --------------------------------------------------
    while (!stop_server) {
        int *client_fd = malloc(sizeof(int));
        if (!client_fd)
            continue;

        *client_fd = accept(server_fd, NULL, NULL);

        if (*client_fd < 0) {
            free(client_fd);
            if (errno == EINTR && stop_server)
                break;
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, client_handler, client_fd);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
