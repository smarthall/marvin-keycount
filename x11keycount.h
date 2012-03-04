#ifndef _x11keycount_H
#define _x11keycount_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BUCKETS 100
#define TIMESEC 300
#define WAITTIME 10000

#define BUCKTIME ((TIMESEC) / (BUCKETS))

typedef struct {
    Display *disp; // X11 Display
    char last_keys[32]; // The last key array from X11
    unsigned int count[BUCKETS]; // The time buckets for the data
    unsigned int max[BUCKETS]; // The time buckets for the data
    int cb; // The current bucket
    time_t nb_time; // The epoch to move to the next bucket after
} x11keycount_t;

x11keycount_t* x11keycount_init();
int x11keycount_close(x11keycount_t *keycount);
unsigned long x11keycount_total(x11keycount_t *keycount);
int x11keycount_max(x11keycount_t *keycount);
float x11keycount_average(x11keycount_t *keycount);
int x11keycount_poll(x11keycount_t *keycount);

#endif

