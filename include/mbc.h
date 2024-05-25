#ifndef MBC_H
#define MBC_H

#include <stdint.h>
#include <stdio.h>

struct cpu;

struct mbc
{
    uint8_t *rom;
    uint8_t *ram;

    uint8_t type;
    uint8_t rom_size;
    uint8_t ram_size;

    unsigned int rom_total_size;
    unsigned int ram_total_size;

    uint16_t rom_bank_count;
    uint8_t ram_bank_count;

    uint8_t rom_bank_number; // AKA BANK1
    uint8_t ram_bank_number; // AKA BANK2

    uint8_t ram_enabled; // AKA RAMG

    uint8_t mbc1_mode; // Banking Mode

    char *rom_path;
    FILE *save_file;
};

void mbc_init(struct mbc *mbc, char *rom_path);

void mbc_free(struct mbc *mbc);

void set_mbc(struct cpu *cpu, uint8_t *rom);

uint8_t read_mbc_rom(struct cpu *cpu, uint16_t address);

uint8_t read_mbc_ram(struct cpu *cpu, uint16_t address);

void write_mbc(struct cpu *cpu, uint16_t address, uint8_t val);

#endif
