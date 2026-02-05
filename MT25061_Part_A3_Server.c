// Roll No: MT25061
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define FIELDS 8

typedef struct {
    int sock;
    size_t msg_size;
} arg_t;

void *handle_client(void *a) {
    arg_t *arg = (arg_t *)a;
    struct iovec iov[FIELDS];
    struct msghdr msg = {0};

    size_t field_size = arg->msg_size / FIELDS;

    for (int i = 0; i < FIELDS; i++) {
        iov[i].iov_base = malloc(field_size);
        iov[i].iov_len = field_size;
        memset(iov[i].iov_base, 'A' + i, field_size);
    }

    msg.msg_iov = iov;
    msg.msg_iovlen = FIELDS;

    while (1) {
        if (sendmsg(arg->sock, &msg, MSG_ZEROCOPY) <= 0)
            break;
    }

    for (int i = 0; i < FIELDS; i++)
        free(iov[i].iov_base);

    close(arg->sock);
    free(arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    size_t msg_size = atoi(argv[1]);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    int enable = 1;
    setsockopt(s, SOL_SOCKET, SO_ZEROCOPY, &enable, sizeof(enable));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    listen(s, 128);

    while (1) {
        int c = accept(s, NULL, NULL);
        arg_t *a = malloc(sizeof(*a));
        a->sock = c;
        a->msg_size = msg_size;

        pthread_t t;
        pthread_create(&t, NULL, handle_client, a);
        pthread_detach(t);
    }
}
