#include "raspio.h"

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

void *raw_time, *raw_gpio, *raw_intr;
volatile uint32_t *time_lo, *time_hi;
volatile uint32_t *intrupt;

unsigned int old_intr[3] = {0, 0, 0};

#define BLOCK_SIZE (4096)

int piinit() {
    int devmem;

    devmem = open("/dev/mem", O_RDWR | O_SYNC);
    if (devmem < 0) {
        printf("Error opening /dev/mem\n");
        return 0;
    }

    raw_time = mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, devmem, 0x20003000);
    raw_gpio = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0x20200000);
    raw_intr = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0x2000B000);

    close(devmem);

    if (raw_time == MAP_FAILED ||
        raw_gpio == MAP_FAILED ||
        raw_intr == MAP_FAILED) {
        printf("Error while mapping timer memory\n");
        return 0;
    }

    time_lo = (volatile uint32_t *) raw_time;
    time_lo++;

    time_hi = time_lo;
    time_hi++;

    intrupt = (volatile uint32_t *)raw_intr;

    return 1;
}

uint64_t pitime() {
    uint32_t tlo = *time_lo;
    uint32_t thi = *time_hi;
    
    uint64_t time = (uint64_t)tlo + ((uint64_t)thi << 32);

    return time;
}

void pidelay(int delay_microseconds) {
    uint64_t t0 = pitime();
    uint64_t t1 = t0 + delay_microseconds;

    while (pitime() < t1);
}

/*
    BCM2835 interrupt registers

    0x200 = 512 (128 * 4) IRQ basic pending
    0x204 = 516 (129 * 4) IRQ pending 1
    0x208 = 520 (130 * 4) IRQ pending 2
    0x20C = 524 (131 * 4) FIQ control
    0x210 = 528 (132 * 4) Enable IRQs 1
    0x214 = 532 (133 * 4) Enable IRQs 2
    0x218 = 536 (134 * 4) Enable Basic IRQs
    0x21C = 540 (135 * 4) Disable IRQs 1
    0x220 = 544 (136 * 4) Disable IRQs 2
    0x224 = 548 (137 * 4) Disable Basic IRQs
*/

int disable_interrupts() {
    if(old_intr[0]) {
        return 0;
    }

    if(*(intrupt + 128) | *(intrupt + 129) | *(intrupt + 130)) {
        return 0; // there are pending interrupts
    }

    old_intr[2] = *(intrupt + 134);
    *(intrupt + 137) = old_intr[2];

    old_intr[0] = *(intrupt + 132);
    *(intrupt + 135) = old_intr[0];

    old_intr[1] = *(intrupt + 133);
    *(intrupt + 136) = old_intr[1];

    return 1;
}

int enable_interrupts() {
    if(old_intr[0] == 0) { // nothing to do
        return 0;
    }

    *(intrupt + 132) = old_intr[0];
    *(intrupt + 133) = old_intr[1];
    *(intrupt + 134) = old_intr[2];

    old_intr[0] = 0;

    return 1;
}
