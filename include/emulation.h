#ifndef EMULATION_H
#define EMULATION_H

#include <stdint.h>

struct cpu;
struct ppu;

void main_loop(struct cpu *cpu, char *rom_path);

void tick_m(struct cpu *cpu);

uint8_t read_mem(struct cpu *cpu, uint16_t address);

void write_mem(struct cpu *cpu, uint16_t address, uint8_t val);

uint8_t read_mem_no_tick(struct cpu *cpu, uint16_t address);

#endif
