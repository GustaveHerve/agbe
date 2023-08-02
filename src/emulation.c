#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "ppu.h"
#include "utils.h"

void main_loop()
{

}

void tick(struct cpu *cpu)
{
    //TODO advance one t-state on everything other than CPU
}

void tick_m(struct cpu *cpu)
{
    //TODO advance one MCycle (4 t-states) on everything other than CPU
}

uint8_t read_mem(struct cpu *cpu, uint16_t address)
{
    //TODO Verify that address is valid
    //TODO PPU tick +1 MCycle (+4 T-State)

    //VRAM read
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (cpu->ppu->vram_locked)
            return 0xFF;
    }

    //OAM read
    else if (address >= 0xFE00 && address <= 0xFE9F)
    {
        if (cpu->ppu->oam_locked)
            return 0xFF;
    }

    tick_m(cpu);
    return cpu->membus[address];
}

void write_mem(struct cpu *cpu, uint16_t address, uint8_t val)
{
    //TODO Verify that address is valid
    int write = 1;
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (cpu->ppu->vram_locked)
            write = 0;
    }

    //OAM read
    else if (address >= 0xFE00 && address <= 0xFE9F)
    {
        if (cpu->ppu->oam_locked)
            write = 0;
    }
    tick_m(cpu);
    //TODO PPU tick +1 MCycle (+4 T-State)
    if (write)
        cpu->membus[address] = val;
}

/*
int address_valid(struct cpu *cpu, uint16_t address)
{
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        reutr
    }
    else if (address >= 0xFE00 && address <= 0xFE9F)
    {
        if (cpu->ppu->oam_locked)
            return 0;
    }
    */
