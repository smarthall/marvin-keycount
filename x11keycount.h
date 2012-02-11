#ifndef _x11keycount_H
#define _x11keycount_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
    Display *disp;
    char last_keys[32];
} x11keycount_t;

x11keycount_t* x11keycount_init();
int x11keycount_close(x11keycount_t *keycount);
int x11keycount_count(x11keycount_t *keycount, int *count);

#endif

