#include "raspio.h"

#include <stdio.h>

int main() {
    piinit();

    uint64_t t = pitime();
    printf("Current Pi time: %llu microseconds\n", t);

    printf("Delaying 1 second...\n");
    pidelay(1000000);
    printf("\tdone\n");

    piclose();

    return 0;
}
