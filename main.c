#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "x11keycount.h"
#include "tcpserver.h"

int main() {
    x11keycount_t *keycount;
    tcpserver_t *server;

    keycount = x11keycount_init();
    server = tcpserver_init();

    while (1) {
        x11keycount_poll(keycount);
        tcpserver_sendreplies(server, WAITTIME, x11keycount_total(keycount));
    }

    tcpserver_close(server);
    server = NULL;
    x11keycount_close(keycount);
    keycount = NULL;
}

