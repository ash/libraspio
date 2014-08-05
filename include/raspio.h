#ifndef __RASPIO_H__
#define __RASPIO_H__

#include <stdint.h>

int piinit();

uint64_t pitime();
void pidelay(int delay_microseconds);

#endif
