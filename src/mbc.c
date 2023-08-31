#include <stdint.h>
#include "cpu.h"
#include "mbc.h"

void set_mbc(struct cpu *cpu)
{
    cpu->mbc->type = cpu->membus[0x0147];
    cpu->mbc->rom_size = cpu->membus[0x0148];
    cpu->mbc->ram_size = cpu->membus[0x0149];
    int size = 0;
    switch (cpu->mbc->rom_size)
    {
        case 0x00:
            cpu->mbc->rom_bank_count = 2;
            break;
        case 0x01:
            cpu->mbc->rom_bank_count = 4;
            break;
        case 0x02:
            cpu->mbc->rom_bank_count = 8;
            break;
        case 0x03:
            cpu->mbc->rom_bank_count = 16;
            break;
        case 0x04:
            cpu->mbc->rom_bank_count = 32;
            break;
        case 0x05:
            cpu->mbc->rom_bank_count = 64;
            break;
        case 0x06:
            cpu->mbc->rom_bank_count = 128;
            break;
        case 0x07:
            cpu->mbc->rom_bank_count = 256;
            break;
        case 0x08:
            cpu->mbc->rom_bank_count = 512;
            break;
    }
    size = 16384 * cpu->mbc->rom_bank_count;
    if (cpu->rom != NULL)
        free(cpu->rom);
    cpu->rom = malloc(sizeof(uint8_t) * size);
}

void write_mbc(struct cpu *cpu, uint16_t address, uint8_t val)
{
    //RAM Enable
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        if (cpu->mbc->type == 0x01)
        {
            if ((val & 0x0F) == 0x0A)
                cpu->mbc->ram_enabled = 1;
            else
                cpu->mbc->ram_enabled = 0;
        }
    }
    else if (address >= 0x2000 && address <= 0x3FFF)
    {
        uint8_t bank = val & 0x1F;
        //Prevent bank 0x00 duplication... (only if uses 5 bits)
        if (bank == 0x00)
            cpu->mbc->bank_selected = 0x01;
        else
        {
            uint8_t mask = cpu->mbc->rom_bank_count - 1;
            cpu->mbc->bank_selected = val & mask;
        }
        rom_bank(cpu, cpu->mbc->bank_selected);
    }
    else if (address >= 0x4000 && address <= 0x5FFF)
    {
        //TODO
    }
    else if (address >= 0x6000 && address <= 0x7FFF)
    {
        //TODO
    }
}

//Emulates bank switching in membus range 0x4000-0x7FFF
void rom_bank(struct cpu *cpu, uint8_t bank)
{
    memcpy(cpu->membus + 0x4000, cpu->rom + 0x4000 * bank, 16384);
}
