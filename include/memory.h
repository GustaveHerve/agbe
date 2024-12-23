#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
struct cpu;

void write_mem(struct cpu *cpu, uint16_t address, uint8_t val);

uint8_t read_mem(struct cpu *cpu, uint16_t address);

uint8_t read_mem_tick(struct cpu *cpu, uint16_t address);

#endif
