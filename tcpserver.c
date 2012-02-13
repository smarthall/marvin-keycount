#define _XOPEN_SOURCE 500
#include "tcpserver.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>

tcpserver_t *tcpserver_init() {
    struct sockaddr_in servaddr;
    tcpserver_t *server;

    server = malloc(sizeof(tcpserver_t));
    server->list_s = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(DEFAULT_PORT);
    bind(server->list_s, (struct sockaddr*) &servaddr, sizeof(servaddr));
    listen(server->list_s, WAITING_CONN);

    return server;
}

int tcpserver_sendreplies(tcpserver_t *server, int timeout, unsigned long count) {
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
        fprintf(outstream, "%ld\n", count);
	count = 0;
	fclose(outstream);
	close(csock);
    }

    return count;
}

int tcpserver_close(tcpserver_t *server) {
    close(server->list_s);
    free(server);
    return EXIT_SUCCESS;
}


