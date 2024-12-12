#include "mbc/mbc_base.h"

#include <stdlib.h>

#include "cpu.h"

void mbc_init(struct mbc_base *mbc, char *rom_path)
{
    mbc->rom = NULL;
    mbc->ram = NULL;

    mbc->ram_size = 0;
    mbc->rom_size = 0;

    mbc->rom_bank_count = 0;
    mbc->ram_bank_count = 0;

    mbc->rom_path = rom_path;
    mbc->save_file = NULL;

    mbc->_mbc_init(mbc);
}

void mbc_free(struct mbc_base *mbc)
{
    mbc->_mbc_free(mbc);

    if (mbc->save_file)
        fclose(mbc->save_file);
    free(mbc->rom);
    free(mbc->ram);
}

void set_mbc(struct cpu *cpu, uint8_t *rom)
{
}

uint8_t read_mbc_rom(struct cpu *cpu, uint16_t address)
{
    return cpu->mbc_g->_read_mbc_rom(cpu, address);
}

uint8_t read_mbc_ram(struct cpu *cpu, uint16_t address)
{
    return cpu->mbc_g->_read_mbc_ram(cpu, address);
}

void write_mbc_ram(struct cpu *cpu, uint16_t address, uint8_t val)
{
    cpu->mbc_g->_write_mbc_ram(cpu, address, val);
}

void write_mbc(struct cpu *cpu, uint16_t address, uint8_t val)
{
    cpu->mbc_g->_write_mbc_ram(cpu, address, val);
}
