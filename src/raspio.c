#include "raspio.h"

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

void *raw_timer, *raw_gpio;
volatile uint32_t *timer_lo, *timer_hi;

#define BLOCK_SIZE (4096)

int piinit() {
    int devmem;

    devmem = open("/dev/mem", O_RDWR | O_SYNC);
    if (devmem < 0) {
        printf("Error opening /dev/mem\n");
        return 0;
    }

    raw_timer = mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, devmem, 0x20003000);
    raw_gpio = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0x20200000);

    close(devmem);

    if (raw_timer == MAP_FAILED ||
        raw_gpio == MAP_FAILED) {
        printf("Error while mapping timer memory\n");
        return 0;
    }

    timer_lo = (volatile uint32_t *) raw_timer;
    timer_lo++;

    timer_hi = timer_lo;
    timer_hi++;

    return 1;
}

int piclose() {
    unmapmem((void**) &raw_timer, BLOCK_SIZE);
    unmapmem((void**) &raw_gpio, BLOCK_SIZE);
}

uint64_t pitime() {
    uint32_t tlo = *timer_lo;
    uint32_t thi = *timer_hi;
    
    uint64_t time = (uint64_t)tlo + ((uint64_t)thi << 32);

    return time;
}

void pidelay(int delay_microseconds) {
    uint64_t t0 = pitime();
    uint64_t t1 = t0 + delay_microseconds;

    while (pitime() < t1);
}
