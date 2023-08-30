#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include <errno.h>

#define MAX_CLIENTS 3

int clients;
pthread_t threads[MAX_CLIENTS];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    struct sockaddr *addr;
    int socket_fd;
} connection_t;

extern int errno;
void *handle_connection(void* peer_data) {
    connection_t *peer = (connection_t*) peer_data;
    int max = 100;
    char buff[max];
    for (;;) {
        int x = read(peer->socket_fd, buff, max);

        if (x == -1) {
            fprintf(stderr, "Value of errno: %d\n", errno);
            perror("Error printed by perror");
            goto cleanup;
        } else if (x == 0) {
            printf("Socket connection terminated.\n");
            goto cleanup;
        }

        printf("%s", buff);
        memset(&buff[0], 0, sizeof(buff));
    }

cleanup:
    pthread_mutex_lock(&mutex);
    
    free(peer);
    clients--;
    printf("%d clients\n", clients);

    pthread_mutex_unlock(&mutex);
    return 0;
}

int main(void) {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Mutex init failed\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8096);
    serv.sin_addr.s_addr = INADDR_ANY;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int limit = 5;
    bind(fd, (struct sockaddr *)&serv, sizeof(serv));
    listen(fd, limit);

    clients = 0;
    for (;;) {
        struct sockaddr peer;
        socklen_t size = sizeof(peer);
        int socket = accept(fd, &peer, &size);

        if (socket == -1) {
            printf("Error connecting new peer.\n");
            continue;
        }

        connection_t *conn_data = malloc(sizeof(connection_t));
        conn_data->addr = &peer;
        conn_data->socket_fd = socket;

        pthread_create(&threads[clients], NULL, handle_connection, (void *)conn_data);
        clients++;

        printf("%d clients\n", clients);

        if (clients == MAX_CLIENTS)
            break;
    }

    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}