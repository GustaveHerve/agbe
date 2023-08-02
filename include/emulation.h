#ifndef EMULATION_H
#define EMULATION_H

#include <stdlib.h>
#include "cpu.h"
#include "ppu.h"

void tick(struct cpu *cpu);
void tick_m(struct cpu *cpu);

uint8_t read_mem(struct cpu *cpu, uint16_t address);
void write_mem(struct cpu *cpu, uint16_t address, uint8_t val);
int address_valid(struct cpu *cpu, uint16_t address);

#endif
