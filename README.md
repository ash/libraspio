libraspio
=========

Raspberry Pi GPIO access library

Available functions:

uint64_t pitime() — get current time from the system timer.

void pidelay(int delay_microseconds) — precise delay in microseconds.

int disable_interrupts() - disables software interrupts and makes your Raspberry Pi kind of real-time system.

int enable_interrupts() - enables interrupts (network works again).
