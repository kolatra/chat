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
char message[MAX];

bool quit(char* msg) {
    if (strcmp(msg, "q\r\n") == 0 || strcmp(msg, "q\n") == 0) {
        return true;
    }
    return false;
}

int main(void) {
    fd = socket(AF_INET, SOCK_STREAM, 0);

    serv.sin_family = AF_INET;
    serv.sin_port = htons(8096);

    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);

    int c = connect(fd, (struct sockaddr *)&serv, sizeof(serv));

    if (c == -1) {
        printf("Error connecting to server, is it up?\n");
        return EXIT_FAILURE;
    }

    for (;;) {
        printf("Enter a message: ");
        fgets(message, MAX, stdin);

        if (quit(message))
            break;
        
        send(fd, message, strlen(message), 0);

        memset(&message[0], 0, sizeof(message));
    }

    close(fd);
    return 0;
}
