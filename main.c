#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "x11keycount.h"
#include "tcpserver.h"

int tcpcallback(char *command, char *reply, int bufsize);

int main() {
    x11keycount_t *keycount;
    tcpserver_t *server;

    keycount = x11keycount_init();
    server = tcpserver_init();
    tcpserver_setcallback(server, &tcpcallback);

    while (1) {
        x11keycount_poll(keycount);
        tcpserver_handle(server, WAITTIME);
    }

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

    strcpy(reply, "Unknown Command\n");
    return EXIT_SUCCESS;
}

