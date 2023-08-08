#include <stdlib.h>
#include <SDL2/SDL.h>
#include <err.h>
#include "cpu.h"
#include "ppu.h"
#include "utils.h"
#include "disassembler.h"
#include "emulation.h"

void main_loop(struct cpu *cpu)
{
    tick_m(cpu);
    int mcycles = next_op(cpu);
    check_interrupt(cpu);
    //TODO add interrupt verification
}

void tick_m(struct cpu *cpu)
{
    ppu_tick_m(cpu->ppu);
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
    else if (address >= 0xFE00 && address <= 0xFEFF)
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
    else if (address >= 0xFE00 && address <= 0xFEFF)
    {
        if (cpu->ppu->oam_locked)
            write = 0;
    }
    else if (address == 0xFF04)
        cpu->div = 0;
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
