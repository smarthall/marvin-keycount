#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "x11keycount.h"
#include "tcpserver.h"

#define BUCKETS 100
#define TIMESEC 300
#define WAITTIME 10000

#define BUCKTIME ((TIMESEC) / (BUCKETS))

int main() {
    x11keycount_t *keycount;
    tcpserver_t *server;
    int count;
    unsigned int countbuckets[BUCKETS];
    int currentbucket = 0;
    time_t newbucket;

    keycount = x11keycount_init();
    server = tcpserver_init();
    newbucket = time(NULL) + BUCKTIME;

    for (int i = 0; i < BUCKETS; i++) countbuckets[i] = 0;

    while (1) {
        x11keycount_count(keycount, &count);
	countbuckets[currentbucket] += count;
	tcpserver_sendreplies(server, WAITTIME, countbuckets);

	if (newbucket < time(NULL)) {
           currentbucket = (currentbucket + 1) % BUCKETS;
           countbuckets[currentbucket] = 0;
           newbucket = time(NULL) + BUCKTIME;
        }
    }

    tcpserver_close(server);
    server = NULL;
    x11keycount_close(keycount);
    keycount = NULL;
}

