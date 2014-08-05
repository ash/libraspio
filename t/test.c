#include "raspio.h"

#include <stdio.h>

int main() {
    piinit();

    uint64_t t = pitime();
    printf("Current Pi time: %llu microseconds\n", t);


    printf("Delaying 1 second...\n");
    pidelay(1000000);
    printf("\tdone\n");


    printf("Disabling interrupts and delay for 3 seconds (no keyboard reaction during this time)\n");
    disable_interrupts();

    pidelay(3000000);

    printf("Enabling interrupts\n");
    enable_interrupts();


    return 0;
}
