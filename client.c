#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX 100

struct sockaddr_in serv;
int fd;
int conn;

int connect_to(char* ip, int port) {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serv.sin_addr);
    return connect(fd, (struct sockaddr *)&serv, sizeof(serv));
}

bool quit(char* msg) {
    // Windows lol
    if (strcmp(msg, "q\r\n") == 0 || strcmp(msg, "q\n") == 0) {
        return true;
    }
    return false;
}

int handle_message(char* msg) {
    if (quit(msg))
        return -1;

    send(fd, msg, strlen(msg), 0);
    memset(&msg[0], 0, sizeof(*msg));
    return 0;
}

int main(void) {
    char username[10];
    printf("Enter a username (max 10 characters): ");
    fgets(username, 10, stdin);

    if (connect_to("127.0.0.1", 8096) == -1) {
        printf("Error connecting to server, is it up?\n");
        return EXIT_FAILURE;
    }

    send(fd, username, sizeof(username), 0);

    char message[MAX];
    for (;;) {
        printf("Enter a message: ");
        fgets(message, MAX, stdin);

        if (handle_message(message) == -1)
            break;
    }

    close(fd);
    return 0;
}
