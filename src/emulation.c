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
    if (!cpu->stop)
    {
        cpu->div16 += 1;
        *cpu->div = (cpu->div16 >> 6) & 0xFF;
    }
    uint8_t previous = *cpu->tima;
    if (*cpu->tac >> 2 & 0x01)
    {
        int temp = 0;
        switch ((*cpu->tac | 0x03))
        {
            case 0:
                temp = 256;
                break;
            case 1:
                temp = 4;
                break;
            case 2:
                temp = 16;
                break;
            case 3:
                temp = 64;
                break;
        }

        if (cpu->acc_timer >= temp)
            *cpu->tima += 1;
    }

    //Overflow
    if (previous > *cpu->tima)
    {
        *cpu->tima = *cpu->tma;
        set_if(cpu, 2);
    }

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
    {
        *cpu->div = 0;
        cpu->div16 = 0;
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
