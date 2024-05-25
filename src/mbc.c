#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "mbc.h"
#include "save.h"

void mbc_init(struct mbc *mbc, char *rom_path)
{
    mbc->rom = NULL;
    mbc->ram = NULL;

    mbc->rom_bank_number = 1;
    mbc->ram_bank_number = 0;

    mbc->rom_bank_count = 0;
    mbc->ram_bank_count = 0;

    mbc->ram_enabled = 0;
    mbc->ram_size = 0;
    mbc->rom_size = 0;

    mbc->mbc1_mode = 0;

    mbc->rom_path = rom_path;
    mbc->save_file = NULL;
}

void mbc_free(struct mbc *mbc)
{
    fclose(mbc->save_file);
    free(mbc->rom);
    free(mbc->ram);
    free(mbc);
}

void set_mbc(struct cpu *cpu, uint8_t *rom)
{
    struct mbc *mbc = cpu->mbc;

    mbc->rom = rom;

    mbc->type = rom[0x0147];

    mbc->rom_size = rom[0x0148];
    mbc->ram_size = rom[0x0149];

    mbc->rom_bank_count = pow(2, cpu->mbc->rom_size + 1);
    mbc->rom_bank_number = 1;

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

    mbc->ram_bank_number = 0;

    mbc->rom_total_size = mbc->rom_bank_count * 16384;
    mbc->ram_total_size = mbc->ram_bank_count * 8192;

    // Allocate the external RAM
    free(mbc->ram);
    mbc->ram = calloc(8192 * mbc->ram_bank_count, sizeof(uint8_t));

    // Create / Load save file if battery
    if (mbc->type == 0x03 || mbc->type == 0x06 || mbc->type == 0x09 || mbc->type == 0x0D || mbc->type == 0x0F || mbc->type == 0x10 || mbc->type == 0x13 || mbc->type == 0x1B || mbc->type == 0x1E || mbc->type == 0x22 || mbc->type == 0xFF)
        mbc->save_file = open_save_file(mbc);
}

uint8_t read_mbc_rom(struct cpu *cpu, uint16_t address)
{
    unsigned int res_addr = address & 0x3FFF;

    if (address <= 0x3FFF)
    {
        if (cpu->mbc->mbc1_mode)
            res_addr = (cpu->mbc->ram_bank_number << 19) | res_addr;
    }

    else if (address >= 0x4000 && address <= 0x7FFF)
        res_addr = (cpu->mbc->ram_bank_number << 19) | (cpu->mbc->rom_bank_number << 14) | res_addr;

    // If address is too big for the size of ROM, only keep necessary bit count
    // to address total ROM size values
    unsigned int mask = 0x00100000;
    while (res_addr > cpu->mbc->rom_total_size)
    {
        res_addr &= (~mask);
        mask >>= 1;
    }

    return cpu->mbc->rom[res_addr];
}

uint8_t read_mbc_ram(struct cpu *cpu, uint16_t address)
{
    if (!cpu->mbc->ram_enabled || cpu->mbc->ram_bank_count == 0)
        return 0xFF;
    unsigned int res_addr = address & 0x1FFF;
    if (cpu->mbc->mbc1_mode)
        res_addr = (cpu->mbc->ram_bank_number << 13) | res_addr;

    return cpu->mbc->ram[res_addr];
}

void write_mbc_ram(struct cpu *cpu, uint16_t address, uint8_t val)
{
    unsigned int res_addr = address & 0x1FFF;
    if (cpu->mbc->mbc1_mode)
        res_addr = (cpu->mbc->ram_bank_number << 13) | res_addr;
    cpu->mbc->ram[res_addr] = val;
}

void write_mbc(struct cpu *cpu, uint16_t address, uint8_t val)
{
    struct mbc *mbc = cpu->mbc;
    // RAM Enable
    if (address <= 0x1FFF)
    {
        if (mbc->type >= 0x01 && mbc->type <= 0x03)
        {
            if ((val & 0x0F) == 0x0A)
                mbc->ram_enabled = 1;
            else
                mbc->ram_enabled = 0;
        }
    }

    else if (address >= 0x2000 && address <= 0x3FFF)
    {
        uint8_t bank = val & 0x1F;
        // Prevent bank 0x00 duplication... (only if uses 5 bits)
        if (bank == 0x00)
            mbc->rom_bank_number = 0x01;
        else
        {
            uint8_t mask = mbc->rom_bank_count - 1;
            mbc->rom_bank_number = val & mask;
        }
        // rom_bank(cpu, mbc->rom_bank_number);
    }

    // RAM bank switch OR Upper bits of ROM bank switch
    else if (address >= 0x4000 && address <= 0x5FFF)
    {
        uint8_t bank = val & 0x03;
        // RAM bank switch
        // RAM size needs to be 32 kiB (4 banks of 8 kiB)
        if (mbc->ram_bank_count == 4)
            mbc->ram_bank_number = bank;
        // TODO: Upper bits of ROM bank switch
    }

    // Banking mode select
    else if (address >= 0x6000 && address <= 0x7FFF)
    {
        if (mbc->rom_bank_count <= 32 && mbc->ram_bank_count <= 1)
            return;
        mbc->mbc1_mode = val & 0x01;
    }

    // External RAM
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        // Ignore writes if RAM is disabled or if there is no external RAM
        if (!mbc->ram_enabled || mbc->ram_bank_count == 0)
            return;

        write_mbc_ram(cpu, address, val);
        //uint16_t res_addr = (address - 0xA000) + 0x2000 * mbc->ram_bank_number;
        //mbc->ram[res_addr] = val;

        // Save if MBC has a save battery
        if (mbc->save_file != NULL)
            save_ram_to_file(mbc);
    }
}
