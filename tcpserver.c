#define _XOPEN_SOURCE 500
#include "tcpserver.h"
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

tcpserver_t *tcpserver_init() {
    struct sockaddr_in servaddr;
    tcpserver_t *server;

    server = malloc(sizeof(tcpserver_t));
    server->list_s = socket(AF_INET, SOCK_STREAM, 0);
    server->tcpcallback = NULL;
    server->openedcount = 0;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(DEFAULT_PORT);
    bind(server->list_s, (struct sockaddr*) &servaddr, sizeof(servaddr));
    listen(server->list_s, WAITING_CONN);
    fcntl(server->list_s, F_SETFD, O_NONBLOCK);

    return server;
}

int tcpserver_sendreplies(tcpserver_t *server, int timeout, unsigned int count) {
    fd_set socks;
    struct timeval time;
    int csock;
    FILE *outstream;

    time.tv_sec = 0;
    time.tv_usec = timeout;
    FD_ZERO(&socks);
    FD_SET(server->list_s, &socks);

    while (select(server->list_s + 1, &socks, NULL, NULL, &time) > 0) {
        csock = accept(server->list_s, NULL, NULL);
        outstream = fdopen(csock, "a");
        fprintf(outstream, "%u\n", count);
        fclose(outstream);
        close(csock);
    }

    return EXIT_SUCCESS;
}

int tcpserver_close(tcpserver_t *server) {
    close(server->list_s);
    free(server);
    return EXIT_SUCCESS;
}

int tcpserver_setcallback(tcpserver_t *server,
    int (*tcpcallback)(char*, char*, int)) {
    server->tcpcallback = tcpcallback;
    return EXIT_SUCCESS;
}

int tcpserver_handle(tcpserver_t *server, int timeout) {
    fd_set sockets, origsockets;
    int highfd;
    struct timeval time;

    time.tv_sec = 0;
    time.tv_usec = timeout;

    FD_ZERO(&sockets);
    FD_SET(server->list_s, &sockets);
    highfd = server->list_s;
    for (int i = 0; i < server->openedcount; i++) {
        FD_SET(server->opensocks[i], &sockets);
        if (server->opensocks[i] > highfd) highfd = server->opensocks[i];
    }

    origsockets = sockets;

    while (select(highfd + 1, &sockets, NULL, NULL, &time) > 0) {
        if (FD_ISSET(server->list_s, &sockets)) {
            if (server->openedcount < CONCURRENT_CON) {
                // Accept new connections
                int newsock = server->openedcount;
                server->opensocks[newsock] = accept(server->list_s, NULL, NULL);
                server->cmd_count[newsock] = 0;
                server->openedcount++;
            } else {
                // Refuse the connection
                shutdown(accept(server->list_s, NULL, NULL), 0);
            }
        }
        for (int i = 0; i < server->openedcount; i++) {
            if (FD_ISSET(server->opensocks[i], &sockets)) {
                // Retrieve data from any waiting
                int nb = recv(server->opensocks[i], 
                              server->cmd_buff[i] + server->cmd_count[i],
                              (COMMAND_BUFF - 1) - server->cmd_count[i],
                              0);

                server->cmd_count[i] += nb;
                // TODO What if the lines too long?

                if (strchr(server->cmd_buff[i], '\n')) {
                    char reply_buff[COMMAND_BUFF];
                    server->tcpcallback(server->cmd_buff[i], reply_buff, COMMAND_BUFF);
                    if (strlen(reply_buff) < COMMAND_BUFF)
                        send(server->opensocks[i], reply_buff, strlen(reply_buff) + 1, 0);
                }
            }
        }

        sockets = origsockets;
    }

    return EXIT_SUCCESS;
}

