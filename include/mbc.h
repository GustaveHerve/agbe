#ifndef MBC_H
#define MBC_H

#include <stdint.h>

struct cpu;

struct mbc
{
    uint8_t type;

    uint8_t rom_size;
    uint8_t ram_size;

    uint16_t rom_bank_count;
    uint8_t bank_selected;

    uint8_t ram_enabled;
};

void set_mbc(struct cpu *cpu);
void write_mbc(struct cpu *cpu, uint16_t address, uint8_t val);
void rom_bank(struct cpu *cpu, uint8_t bank);

#endif
