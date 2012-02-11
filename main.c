#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "x11keycount.h"

int main() {
    x11keycount_t *keycount;
    int count, total = 0;

    keycount = x11keycount_init();

    while (1) {
        x11keycount_count(keycount, &count);
	if (count > 0) {
	    total += count;
	    printf("%d\n", total);
	}
        usleep(30000);
    }

    x11keycount_close(keycount);
    keycount = NULL;
}

