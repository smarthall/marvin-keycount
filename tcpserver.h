#ifndef _tcpserver_H
#define _tcpserver_H

#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_PORT 6432
#define WAITING_CONN 10
#define COMMAND_BUFF 255

typedef struct {
    int list_s;
    int (*tcpcallback)(char*, char*, int);
    int openedcount;
    int *opensocks;
    FILE **openstreams;
    char **cmd_buff;
} tcpserver_t;

tcpserver_t *tcpserver_init();
int tcpserver_sendreplies(tcpserver_t *server, int timeout, unsigned int count);
int tcpserver_setcallback(tcpserver_t *server,
    int (*tcpcallback)(char*, char*, int));
int tcpserver_handle(tcpserver_t *server, int timeout);
int tcpserver_close(tcpserver_t *server);

#endif

