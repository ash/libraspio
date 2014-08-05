#include "raspio.h"

#include <stdio.h>

int main() {
    piinit();

    uint64_t t = pitime();

    printf("%llu\n", t);

    return 0;
}
