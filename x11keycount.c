#include "x11keycount.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

x11keycount_t* x11keycount_init() {
    x11keycount_t *keycount;
    int keymapret;

    keycount = calloc(1, sizeof(x11keycount_t));
    if (keycount == NULL) {
        return NULL;
    }

    keycount->disp = XOpenDisplay(NULL);
    if (keycount->disp == NULL) {
        return NULL;
    }

    keymapret = XQueryKeymap(keycount->disp, keycount->last_keys);
    if (keymapret == 0) {
        return NULL;
    }

    keycount->nb_time = time(NULL) + BUCKTIME;

    return keycount;
}

int x11keycount_close(x11keycount_t *keycount) {
    XCloseDisplay(keycount->disp);
    free(keycount);

    return EXIT_SUCCESS;
}

int x11keycount_poll(x11keycount_t *keycount) {
    char keys[32];
    int keymapret;

    keymapret = XQueryKeymap(keycount->disp,keys);
    if (keymapret == 0) {
        return EXIT_FAILURE;
    }
    if (memcmp(keys, keycount->last_keys, sizeof(char) * 32) != 0) {
        for (int i = 0; i < 32; i++) {
            unsigned char diff = keys[i] ^ keycount->last_keys[i];
            // Count different bits
            unsigned char c;
            for (c = 0; diff; diff >>= 1)
            {
                c += diff & 1;
            }
            if (c > 0) {
                keycount->buckets[keycount->cb] += c;
            }
        }
        memcpy(keycount->last_keys, keys, sizeof(char) * 32);
        //TODO: Instead of memcopy, use two arrays and flip
    }

    if (keycount->nb_time < time(NULL)) {
        keycount->cb = (keycount->cb + 1) % BUCKETS;
        keycount->buckets[keycount->cb] = 0;
        keycount->nb_time = time(NULL) + BUCKTIME;
    }

    return EXIT_SUCCESS;
}

unsigned long x11keycount_total(x11keycount_t *keycount) {
    unsigned long count = 0;

    for (int i = 0; i < BUCKETS; i++)
        count += keycount->buckets[i];

    return count;
}

float x11keycount_average(x11keycount_t *keycount) {
    return (float) x11keycount_total(keycount) / TIMESEC;
}

