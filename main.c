#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "x11keycount.h"
#include "tcpserver.h"

x11keycount_t *keycount;
unsigned short int loopexit = 0;

void sigdie(int sig);
int tcpcallback(char *command, char *reply, int bufsize);

int main() {
    tcpserver_t *server;

    keycount = x11keycount_init();
    server = tcpserver_init();
    tcpserver_setcallback(server, &tcpcallback);
    signal(SIGINT, sigdie);

    while (!loopexit) {
        x11keycount_poll(keycount);
        tcpserver_handle(server, WAITTIME);
    }

    printf("Got CTRL-C, Exiting.\n");
    tcpserver_close(server);
    server = NULL;
    x11keycount_close(keycount);
    keycount = NULL;
}

int tcpcallback(char *command, char *reply, int bufsize) {
    if (strncmp("test", command, 4) == 0) {
        strcpy(reply, "ok\n");
        return EXIT_SUCCESS;
    }
    if (strncmp("count", command, 5) == 0) {
        snprintf(reply, bufsize, "%lu\n", x11keycount_total(keycount));
        return EXIT_SUCCESS;
    }
    if (strncmp("avg", command, 3) == 0) {
        snprintf(reply, bufsize, "%f\n", x11keycount_average(keycount));
        return EXIT_SUCCESS;
    }
    if (strncmp("quit", command, 4) == 0) {
        strcpy(reply, "bye\n");
        return EXIT_FAILURE;
    }

    strcpy(reply, "err: unknown\n");
    return EXIT_SUCCESS;
}

void sigdie(int sig) {
    loopexit = 1;
}

