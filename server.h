#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
    struct sockaddr_in addr;
    int port;
    int host_socket;
    int clients;
} Server;

typedef struct {
    Server* server;
    struct sockaddr *addr;
    int client_socket;
    char* username;
} Conn;

void server_init(Server *server, int port);
void *handle_connection(void *arg);
void server_run(Server *server);
