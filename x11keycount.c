#include "x11keycount.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

x11keycount_t* x11keycount_init() {
    x11keycount_t *keycount;

    keycount = malloc(sizeof(x11keycount_t));
    keycount->disp = XOpenDisplay(NULL);
    XQueryKeymap(keycount->disp, keycount->last_keys);

    return keycount;
}

int x11keycount_close(x11keycount_t *keycount) {
    XCloseDisplay(keycount->disp);
    free(keycount);
    
    return EXIT_SUCCESS;
}

int x11keycount_count(x11keycount_t *keycount, int *count) {
    char keys[32];
    *count = 0;

    XQueryKeymap(keycount->disp,keys);
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
                *count += c;
            }
        }
        memcpy(keycount->last_keys, keys, sizeof(char) * 32);
    }

    return EXIT_SUCCESS;
}

