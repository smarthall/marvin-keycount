#ifndef _tcpserver_H
#define _tcpserver_H

#define DEFAULT_PORT 6432
#define CONN_MAX 10

typedef struct {
    int list_s;
    int conn_s[CONN_MAX];
} tcpserver_t;

#endif

