// Roll No: MT25061
// PA02 – Part A Client

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdint.h>

//server configration
#define SERVER_IP "10.0.0.1"
#define SERVER_PORT 8080
/* Fixed experiment duration (seconds) */
#define DURATION_SEC 10

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <message_size> <threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
/* Message size provided at runtime */
    size_t msg_size = atoi(argv[1]);
    if (msg_size == 0) {
        fprintf(stderr, "Invalid message size\n");
        exit(EXIT_FAILURE);
    }
//Allocate receive buffer
    char *buffer = malloc(msg_size);
    if (!buffer) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
//Create TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
//Configure server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
//Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
//Timing and statistics variables
    struct timespec start, end, r1, r2;
    double total_latency_us = 0.0;
    double total_time_sec = 0.0;
    uint64_t total_bytes = 0;
    uint64_t recv_count = 0;
/* Record experiment start time */
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (1) {
        /* Timestamp before recv() */
        clock_gettime(CLOCK_MONOTONIC, &r1);

        ssize_t n = recv(sock, buffer, msg_size, 0);
/* Timestamp after recv() */
        clock_gettime(CLOCK_MONOTONIC, &r2);
/* Stop on connection close or error */
        if (n <= 0)
            break;
/* Compute per-message latency (µs) */
        double latency_us =
            (r2.tv_sec - r1.tv_sec) * 1e6 +
            (r2.tv_nsec - r1.tv_nsec) / 1e3;

        total_latency_us += latency_us;
        total_bytes += n;
        recv_count++;
 /* Record experiment end time */
        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed =
            (end.tv_sec - start.tv_sec) +
            (end.tv_nsec - start.tv_nsec) / 1e9;

        if (elapsed >= DURATION_SEC)
            break;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    total_time_sec =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    double avg_latency_us =
        recv_count ? (total_latency_us / recv_count) : 0.0;

    /* ---- IMPORTANT: machine-readable output for Part C ---- */
    printf("STATS total_time=%lf latency_us=%lf bytes=%lu\n",
           total_time_sec, avg_latency_us, total_bytes);

    fflush(stdout);

    close(sock);
    free(buffer);
    return 0;
}
