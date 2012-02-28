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

#define NULL_POS -1

static int tcpserver_newconnection(tcpserver_t *server, int sock) {
    conn *nc = malloc(sizeof(conn));

    nc->socket = accept(sock, NULL, NULL);
    nc->bufcount = 0;
    server->openedcount++;

    if (server->head == NULL) {
        server->head = nc;
        nc->next = NULL;

        return EXIT_SUCCESS;
    }

    nc->next = server->head;
    server->head = nc;

    return EXIT_SUCCESS;
}

static int tcpserver_killconnection(tcpserver_t *server, int sock) {
    conn *cur = server->head;
    conn *prev = NULL;
    server->openedcount--;

    while ((cur != NULL) && (cur->socket != sock)) {
        prev = cur;
        cur = cur->next;
    }

    if (cur == NULL) return EXIT_FAILURE;

    if (prev != NULL) prev->next = cur->next;
    if (prev == NULL) server->head = cur->next;
    if ((cur == server->cur) && (prev == NULL)) server->cur = NULL;
    if ((cur == server->cur) && (prev != NULL)) server->cur = prev;

    close(cur->socket);
    free(cur);

    return EXIT_SUCCESS;
}

static conn* tcpserver_nextconnection(tcpserver_t *server) {
    if (server->cur == NULL) {
        server->cur = server->head;
        return server->head;
    }

    server->cur = server->cur->next;
    return server->cur;
}

tcpserver_t *tcpserver_init() {
    struct sockaddr_in servaddr;
    tcpserver_t *server;
    int optval = 1;

    server = malloc(sizeof(tcpserver_t));
    server->list_s = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server->list_s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    server->tcpcallback = NULL;
    server->head = NULL;
    server->cur = NULL;
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
    conn *cur;
    struct timeval time;

    time.tv_sec = 0;
    time.tv_usec = timeout;

    /* tcpserver_getfdset */
    FD_ZERO(&sockets);
    FD_SET(server->list_s, &sockets);
    highfd = server->list_s;
    while ((cur = tcpserver_nextconnection(server)) != NULL) {
        FD_SET(cur->socket, &sockets);
        if (cur->socket > highfd) highfd = cur->socket;
    }

    origsockets = sockets;

    while (select(highfd + 1, &sockets, NULL, NULL, &time) > 0) {
        if (FD_ISSET(server->list_s, &sockets))
            tcpserver_newconnection(server, server->list_s);

        while ((cur = tcpserver_nextconnection(server)) != NULL) {
            if (FD_ISSET(cur->socket, &sockets)) {
                /* tcpserver_conngetdata */
                // Retrieve data from any waiting
                int nb = recv(cur->socket, 
                              cur->buf + cur->bufcount,
                              (COMMAND_BUFF - 1) - cur->bufcount,
                              0);

                if (nb <= 0) {
                    tcpserver_killconnection(server, cur->socket);
                    break;
                }

                cur->bufcount += nb;
                // TODO What if the lines too long?

                /* tcpserver_processbuf */
                if (strchr(cur->buf, '\n')) {
                    char rbuf[COMMAND_BUFF];
                    int ret = server->tcpcallback(cur->buf, rbuf, COMMAND_BUFF);
                    if (strlen(rbuf) < COMMAND_BUFF)
                        send(cur->socket, rbuf, strlen(rbuf) + 1, 0);
                    cur->bufcount = 0;
                    if (ret == EXIT_FAILURE) tcpserver_killconnection(server, cur->socket);
                }
            }
        }

        sockets = origsockets;
    }

    return EXIT_SUCCESS;
}

