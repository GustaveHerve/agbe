#ifndef EMULATION_H
#define EMULATION_H

#include <stdint.h>

struct cpu;
struct ppu;

void handle_events(struct cpu *cpu);

void main_loop(struct cpu *cpu, char *rom_path, char *boot_rom_path);

void tick_m(struct cpu *cpu);

#endif
