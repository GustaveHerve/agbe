#ifndef EMULATION_H
#define EMULATION_H

#include <stdlib.h>

struct cpu;
struct ppu;

void main_loop(struct cpu *cpu);
void tick_m(struct cpu *cpu);
void init_cpu(struct cpu *cpu, int checksum);
void init_hardware(struct cpu *cpu);
uint8_t read_mem(struct cpu *cpu, uint16_t address);
void write_mem(struct cpu *cpu, uint16_t address, uint8_t val);
int address_valid(struct cpu *cpu, uint16_t address);

#endif
