#include "raspio.h"

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

void *raw_timer;
volatile uint32_t *timer_lo, *timer_hi;

int piinit() {
    int devmem;

    devmem = open("/dev/mem", O_RDWR | O_SYNC);
    if (devmem < 0) {
        printf("Error opening /dev/mem\n");
        return 0;
    }

    raw_timer = mmap(NULL, 64, PROT_READ, MAP_SHARED, devmem, 0x20003004);

    close(devmem);

    if (raw_timer == MAP_FAILED) {
        printf("Error while mapping timer memory\n");
        return 0;
    }

    timer_lo = (volatile uint32_t *) raw_timer;
    timer_hi = timer_lo;
    timer_hi++;

    return 1;
}

uint64_t pitime() {
    uint32_t tlo = *timer_lo;
    uint32_t thi = *timer_hi;
    
    uint64_t time = (uint64_t)tlo + ((uint64_t)thi << 32);

    return time;
}

void pi_delay(int delay_microseconds) {
    uint64_t t0 = pitime();
    uint64_t t1 = t0 + delay_microseconds;

    while (pitime() < t1);
}
