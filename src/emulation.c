#include <stdlib.h>
#include <stdio.h>
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
    /*
    fptr = fopen("testroms/boot.gb", "rb");
    fread(cpu->membus, 1, 256, fptr);
    fclose(fptr);
    fptr = fopen("testroms/tetris.gb", "rb");
    fseek(fptr, 0x0100, SEEK_SET);
    fread(cpu->membus + 0x100, 1, 80, fptr);
    fclose(fptr);

    //init_cpu(cpu, 0x0a);
    //init_hardware(cpu);

    //First OPCode Fetch
    lcd_off(cpu);
    tick_m(cpu);
    while (cpu->regist->pc != 0x0150)
    {
        next_op(cpu); //Remaining MCycles are ticked in instructions
        tick_m(cpu); // OPCode fetch
        check_interrupt(cpu);
    }
    */
    fptr = fopen("testroms/mario.gb", "rb");
    fread(cpu->membus, 1, 32768, fptr);
    fclose(fptr);

    init_cpu(cpu, 0x0a);
    init_hardware(cpu);
    cpu->membus[0xFF00] = 0xCF;

    lcd_off(cpu);
    tick_m(cpu);
    while (1)
    {
        next_op(cpu); //Remaining MCycles are ticked in instructions
        tick_m(cpu); // OPCode fetch
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
    cpu->membus[0xFF47] = 0xFC; //TODO check this
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

    cpu->ppu->current_mode = 1;
    cpu->ppu->line_dot_count = 400;
    cpu->ppu->mode1_153th = 1;
}

void tick_m(struct cpu *cpu)
{
    if (cpu->ime == 2)
        cpu->ime = 1;

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

    if (get_lcdc(cpu->ppu, 7))
        ppu_tick_m(cpu->ppu);
    else
    {
        cpu->ppu->oam_locked = 0;
        cpu->ppu->vram_locked = 0;
    }
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
    if (address >= 0x0000 && address <= 0x7FFF)
        write = 0;

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
    //JOYP
    else if (address == 0xFF00)
    {
        write = 0;
        uint8_t temp = (cpu->membus[address] & 0xC0);
        temp |= (val & 0x30);
        temp |= (cpu->membus[address] & 0x0F);
        cpu->membus[address] = temp;
    }
    else if (address == 0xFF04)
    {
        *cpu->div = 0;
        cpu->div16 = 0;
    }
    else if (address == 0xFF0F)
    {
        write = 0;
        uint8_t temp = (cpu->membus[address] & 0xE0);
        temp |= (val & 0x1F);
        cpu->membus[address] = temp;
    }
    else if (address == 0xFFFF)
    {
        write = 0;
        uint8_t temp = (cpu->membus[address] & 0xE0);
        temp |= (val & 0x1F);
        cpu->membus[address] = temp;
    }
    tick_m(cpu);
    if (write)
        cpu->membus[address] = val;
}
