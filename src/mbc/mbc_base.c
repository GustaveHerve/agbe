#include "mbc/mbc_base.h"

#include <stdlib.h>

#include "cpu.h"
#include "mbc/no_mbc.h"
#include "save.h"

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

    mbc->_mbc_init = NULL;
    mbc->_mbc_free = NULL;
    mbc->_read_mbc_rom = NULL;
    mbc->_write_mbc_rom = NULL;
    mbc->_read_mbc_ram = NULL;
    mbc->_write_mbc_ram = NULL;
    mbc->_write_mbc = NULL;

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

static struct mbc_base *make_mbc(uint8_t type_byte)
{
    enum MBC_TYPE type = 0;
    struct mbc_base *res = NULL;

    switch (type_byte)
    {
    case 0x00:
        type = 0;
        res = malloc(sizeof(struct no_mbc));
    }

    res->type = type;
    return res;
}

void set_mbc(struct mbc_base **output, uint8_t *rom, char *rom_path)
{
    struct mbc_base *mbc = make_mbc(rom[0x0147]);

    mbc_init(mbc, rom_path);

    mbc->rom = rom;

    mbc->rom_size = rom[0x0148];
    mbc->ram_size = rom[0x0149];

    mbc->rom_bank_count = 1 << (mbc->rom_size + 1);

    switch (mbc->ram_size)
    {
    case 0x00:
        mbc->ram_bank_count = 0;
        break;
    case 0x02:
        mbc->ram_bank_count = 1;
        break;
    case 0x03:
        mbc->ram_bank_count = 4;
        break;
    case 0x04:
        mbc->ram_bank_count = 16;
        break;
    case 0x05:
        mbc->ram_bank_count = 8;
        break;
    }

    mbc->rom_total_size = mbc->rom_bank_count * 16384;
    mbc->ram_total_size = mbc->ram_bank_count * 8192;

    // Allocate the external RAM
    free(mbc->ram);
    mbc->ram = calloc(8192 * mbc->ram_bank_count, sizeof(uint8_t));

    // Create / Load save file if battery
    if (mbc->type == 0x03 || mbc->type == 0x06 || mbc->type == 0x09 || mbc->type == 0x0D || mbc->type == 0x0F ||
        mbc->type == 0x10 || mbc->type == 0x13 || mbc->type == 0x1B || mbc->type == 0x1E || mbc->type == 0x22 ||
        mbc->type == 0xFF)
        mbc->save_file = open_save_file(mbc);

    *output = mbc;
}
uint8_t read_mbc_rom(struct cpu *cpu, uint16_t address)
{
    return cpu->mbc_g->_read_mbc_rom(cpu, address);
}

void write_mbc_rom(struct cpu *cpu, uint16_t address, uint8_t val)
{
    cpu->mbc_g->_write_mbc_rom(cpu, address, val);
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
