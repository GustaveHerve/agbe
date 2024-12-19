#ifndef EMULATION_H
#define EMULATION_H

#include <stdint.h>

struct cpu;
struct ppu;

void main_loop(struct cpu *cpu, char *rom_path, char *boot_rom_path);

void tick_m(struct cpu *cpu);

void write_mem(struct cpu *cpu, uint16_t address, uint8_t val);

uint8_t read_mem(struct cpu *cpu, uint16_t address);

uint8_t read_mem_tick(struct cpu *cpu, uint16_t address);

int64_t get_nanoseconds(void);

void synchronize(struct cpu *cpu);

#endif
