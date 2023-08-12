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
    FILE *fptr;
    fptr = fopen("testroms/tetris.gb", "rb");
    fread(cpu->membus, sizeof(uint8_t), 32768, fptr);
    fclose(fptr);

    init_cpu(cpu, 0x0a);
    init_hardware(cpu);
    init_vram(cpu->ppu);

    while (1)
    {
        tick_m(cpu); // OPCode fetch
        next_op(cpu); //Remaining MCycles are ticked in instructions
        check_interrupt(cpu);
    }
}

void init_cpu(struct cpu *cpu, int checksum)
{
    cpu->regist->a = 0x01;
    set_z(cpu->regist, 1);
    set_n(cpu->regist, 0);
    if (checksum == 0x00)
    {
        set_h(cpu->regist, 0);
        set_c(cpu->regist, 0);
    }
    else
    {
        set_h(cpu->regist, 1);
        set_c(cpu->regist, 1);
    }
    cpu->regist->b = 0x00;
    cpu->regist->c = 0x13;
    cpu->regist->d = 0x00;
    cpu->regist->e = 0xD8;
    cpu->regist->h = 0x01;
    cpu->regist->l = 0x4D;
    cpu->regist->pc = 0x0100;
    cpu->regist->sp = 0xFFFE;
}

void init_hardware(struct cpu *cpu)
{
    cpu->membus[0xFF00] = 0xCF;
    cpu->membus[0xFF01] = 0x00;
    cpu->membus[0xFF02] = 0x7E;
    cpu->membus[0xFF04] = 0xAB;
    cpu->membus[0xFF05] = 0x00;
    cpu->membus[0xFF06] = 0x00;
    cpu->membus[0xFF07] = 0xF8;
    cpu->membus[0xFF0F] = 0xE1;
    cpu->membus[0xFF10] = 0x80;
    cpu->membus[0xFF11] = 0xBF;
    cpu->membus[0xFF12] = 0xF3;
    cpu->membus[0xFF13] = 0xFF;
    cpu->membus[0xFF14] = 0xBF;
    cpu->membus[0xFF16] = 0x3F;
    cpu->membus[0xFF17] = 0x00;
    cpu->membus[0xFF18] = 0xFF;
    cpu->membus[0xFF19] = 0xBF;
    cpu->membus[0xFF1A] = 0x7F;
    cpu->membus[0xFF1B] = 0xFF;
    cpu->membus[0xFF1C] = 0x9F;
    cpu->membus[0xFF1D] = 0xFF;
    cpu->membus[0xFF1E] = 0xBF;
    cpu->membus[0xFF20] = 0xFF;
    cpu->membus[0xFF21] = 0x00;
    cpu->membus[0xFF22] = 0x00;
    cpu->membus[0xFF23] = 0xBF;
    cpu->membus[0xFF24] = 0x77;
    cpu->membus[0xFF25] = 0xF3;
    cpu->membus[0xFF26] = 0xF1;
    cpu->membus[0xFF40] = 0x91;
    cpu->membus[0xFF41] = 0x85;
    cpu->membus[0xFF42] = 0x00;
    cpu->membus[0xFF43] = 0x00;
    cpu->membus[0xFF44] = 0x00;
    cpu->membus[0xFF45] = 0x00;
    cpu->membus[0xFF46] = 0xFF;
    cpu->membus[0xFF47] = 0xFC;
    cpu->membus[0xFF48] = 0xFF; // ?
    cpu->membus[0xFF49] = 0xFF; // ?
    cpu->membus[0xFF4A] = 0x00;
    cpu->membus[0xFF4B] = 0x00;
    cpu->membus[0xFF4D] = 0xFF;
    cpu->membus[0xFF4F] = 0xFF;
    cpu->membus[0xFF51] = 0xFF;
    cpu->membus[0xFF52] = 0xFF;
    cpu->membus[0xFF53] = 0xFF;
    cpu->membus[0xFF54] = 0xFF;
    cpu->membus[0xFF55] = 0xFF;
    cpu->membus[0xFF56] = 0xFF;
    cpu->membus[0xFF68] = 0xFF;
    cpu->membus[0xFF69] = 0xFF;
    cpu->membus[0xFF6A] = 0xFF;
    cpu->membus[0xFF6B] = 0xFF;
    cpu->membus[0xFF70] = 0xFF;
    cpu->membus[0xFFFF] = 0x00;

}

void tick_m(struct cpu *cpu)
{
    if (!cpu->stop)
    {
        cpu->div16 += 1;
        *cpu->div = (cpu->div16 >> 6) & 0xFF;
        cpu->acc_timer += 1;
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
    if (write)
        cpu->membus[address] = val;
}
