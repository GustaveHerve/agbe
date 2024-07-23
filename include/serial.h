#ifndef SERIAL_H
#define SERIAL_H

#include "cpu.h"

static inline uint8_t get_clock_select(struct cpu *cpu)
{
    return *cpu->sc & 0x01;
}

static inline uint8_t get_transfer_enable(struct cpu *cpu)
{
    return (*cpu->sc >> 7) & 0x01;
}

void serial_transfer(struct cpu *cpu);
void transfer_complete(struct cpu *cpu);

#endif
