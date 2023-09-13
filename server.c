#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
extern int errno;
#define MAX_CLIENTS 3
pthread_t threads[MAX_CLIENTS];

void *handle_connection(void* peer_data) {
    Conn *peer = (Conn*) peer_data;
    char buff[100];
    for (;;) {
        int x = read(peer->client_socket, buff, 100);

        if (x == -1) {
            fprintf(stderr, "Value of errno: %d\n", errno);
            perror("Error printed by perror");
            break;
        } else if (x == 0) {
            printf("Socket connection terminated.\n");
            break;
        }

        printf("<%s> %s", peer->username, buff);
        memset(&buff[0], 0, sizeof(buff));
    }

    pthread_mutex_lock(&mutex);
    peer->server->clients--;
    free(peer);
    pthread_mutex_unlock(&mutex);
    return 0;
}

void server_init(Server* server, int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(fd, (struct sockaddr *)&server->addr, sizeof(server->addr));
    server->host_socket = fd;

    server->port = port;
    server->addr.sin_family = AF_INET;
    server->addr.sin_port = htons(port);
    server->addr.sin_addr.s_addr = INADDR_ANY;
}

void server_run(Server* server) {
    listen(server->host_socket, 5);
    for (;;) {
        struct sockaddr peer;
        socklen_t size = sizeof(peer);
        int socket = accept(server->host_socket, &peer, &size);

        if (socket == -1) {
            printf("Error connecting new peer.\n");
            continue;
        }

        char usr_buff[10];
        read(socket, usr_buff, 10);
        // remove any newlines
        usr_buff[strcspn(usr_buff, "\r\n")] = 0;
        printf("username: %s\n", usr_buff);

        Conn *conn_data = malloc(sizeof(Conn));
        conn_data->server = server;
        conn_data->addr = &peer;
        conn_data->client_socket = socket;
        conn_data->username = usr_buff;

        pthread_create(&threads[server->clients], NULL, handle_connection, (void *)conn_data);
        server->clients++;

        printf("%d clients\n", server->clients);
    }
}

int new_main(void) {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Mutex init failed\n");
        return EXIT_FAILURE;
    }

    Server server = {0};
    server_init(&server, 8096);
    set_handler(&server, handle_connection);
    server_run(&server);
    pthread_mutex_destroy(&mutex);
    return 0;
}
