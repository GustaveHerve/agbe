#ifndef EMULATION_H
#define EMULATION_H

#include <stdlib.h>

void tick();
void tick_m();

uint8_t read_mem(struct cpu *cpu, uint16_t address);
void write_mem(struct cpu *cpu, uint16_t address, uint8_t val);

#endif
