#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "x11keycount.h"
#include "tcpserver.h"

int main() {
    x11keycount_t *keycount;
    tcpserver_t *server;
    int count;
    unsigned long total = 0;

    keycount = x11keycount_init();
    server = tcpserver_init();

    while (1) {
        x11keycount_count(keycount, &count);
	total += count;
	total = tcpserver_sendreplies(server, 10000, total);
    }

    tcpserver_close(server);
    server = NULL;
    x11keycount_close(keycount);
    keycount = NULL;
}

