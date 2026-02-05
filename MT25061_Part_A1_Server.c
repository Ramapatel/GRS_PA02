// Roll No: MT25XXX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define FIELDS 8

typedef struct {
    char *fields[FIELDS];
    size_t field_size;
} message_t;

typedef struct {
    int sockfd;
    size_t msg_size;
} thread_arg_t;

void *handle_client(void *arg) {
    thread_arg_t *t = (thread_arg_t *)arg;
    int sock = t->sockfd;
    size_t size = t->msg_size;

    message_t msg;
    msg.field_size = size / FIELDS;

    for (int i = 0; i < FIELDS; i++) {
        msg.fields[i] = malloc(msg.field_size);
        memset(msg.fields[i], 'A' + i, msg.field_size);
    }

    while (1) {
        for (int i = 0; i < FIELDS; i++) {
            if (send(sock, msg.fields[i], msg.field_size, 0) <= 0)
                goto cleanup;
        }
    }

cleanup:
    for (int i = 0; i < FIELDS; i++)
        free(msg.fields[i]);
    close(sock);
    free(t);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <msg_size> <threads>\n", argv[0]);
        return 1;
    }

    size_t msg_size = atoi(argv[1]);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 128);

    while (1) {
        int client = accept(server_fd, NULL, NULL);
        thread_arg_t *arg = malloc(sizeof(*arg));
        arg->sockfd = client;
        arg->msg_size = msg_size;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, arg);
        pthread_detach(tid);
    }
}
