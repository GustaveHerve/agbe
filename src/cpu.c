#include <stdlib.h>
#include "cpu.h"
#include "ppu.h"
#include "control.h"
#include "jump.h"
#include "load.h"
#include "utils.h"
#include "emulation.h"
#include "mbc.h"

#define MEMBUS_SIZE 65536 // In bytes

int get_ie(struct cpu *cpu, int bit);

void cpu_init(struct cpu *cpu, struct renderer *rend)
{
	cpu->regist = malloc(sizeof(struct cpu_register));
	cpu->membus = calloc(MEMBUS_SIZE, sizeof(uint8_t));
    cpu->rom = NULL;
    cpu->ppu = malloc(sizeof(struct ppu));
    ppu_init(cpu->ppu, cpu, rend);
    cpu->ime = 0;
    cpu->ie = &cpu->membus[0xFFFF];
    cpu->_if = &cpu->membus[0xFF0F];
    cpu->div = &cpu->membus[0xFF04];
    cpu->tima = &cpu->membus[0xFF05];
    cpu->tma = &cpu->membus[0xFF06];
    cpu->tac = &cpu->membus[0xFF07];
    cpu->halt = 0;
    cpu->stop = 0;


    cpu->div_timer = 0;
    cpu->tima_timer = 0;

    cpu->mbc = malloc(sizeof(struct mbc));

    //Joypad
    cpu->joyp_a = 0xFF;
    cpu->joyp_d = 0xFF;

    //Values BEFORE bootrom
    cpu->regist->a = 0x00;
    cpu->regist->f = 0x00;
    cpu->regist->b = 0x00;
    cpu->regist->c = 0x00;
    cpu->regist->h = 0x00;
    cpu->regist->l = 0x00;
    cpu->regist->pc = 0x00;
    cpu->regist->sp = 0x00;

    *cpu->ie = 0x00;
    *cpu->_if = 0xE1;
    *cpu->div = 0x00;
    *cpu->tima = 0x00;
    *cpu->tma = 0x00;
    *cpu->tac = 0x00;
}

//Set registers' default values AFTER boot rom
void cpu_init_regist(struct cpu *cpu)
{
    cpu->regist->a = 0x01;
    cpu->regist->f = 0xB0;
    cpu->regist->b = 0x00;
    cpu->regist->c = 0x13;
    cpu->regist->d = 0x00;
    cpu->regist->e = 0xD8;
    cpu->regist->h = 0x01;
    cpu->regist->l = 0x4D;
    cpu->regist->sp = 0xFFFE;
    cpu->regist->pc = 0x0100;
}

void cpu_free(struct cpu *todelete)
{
    ppu_free(todelete->ppu);
	free(todelete->membus);
    if (todelete->rom != NULL)
        free(todelete->rom);
    free(todelete->regist);
    free(todelete->mbc);
	free(todelete);
}

//Interrupt handling
int check_interrupt(struct cpu *cpu)
{
    if (!cpu->ime)
        return 0;

    //Joypad check
    if (((cpu->membus[0xFF00] >> 5 & 0x01) == 0x00) ||
        (cpu->membus[0xFF00] >> 4 & 0x01) == 0x00)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (((cpu->membus[0xFF00] >> 5 & 0x01) == 0x00) ||
                (cpu->membus[0xFF00] >> 4 & 0x01) == 0x00)
            {
                if (((cpu->membus[0xFF00] >> i) & 0x01) == 0x00)
                    set_if(cpu, 4);
            }
        }
    }

    for (int i = 0; i < 5; ++i)
    {
        if (get_if(cpu, i) && get_ie(cpu, i))
        {
            cpu->halt = 0;
            handle_interrupt(cpu, i);
        }
    }
    return 1;
}

int handle_interrupt(struct cpu *cpu, int bit)
{
    clear_if(cpu, bit);
    cpu->ime = 0;
    tick_m(cpu);
    tick_m(cpu);
    uint8_t lo = regist_lo(&cpu->regist->pc);
    uint8_t hi = regist_hi(&cpu->regist->pc);
    --cpu->regist->sp;
    write_mem(cpu, cpu->regist->sp, hi);
    --cpu->regist->sp;
    write_mem(cpu, cpu->regist->sp, lo);
    uint16_t handler = 0;
    switch (bit)
    {
        // VBlank
        case 0:
            handler = 0x40;
            break;
        // LCD STAT
        case 1:
            handler = 0x48;
            break;
        // Timer
        case 2:
            handler = 0x50;
            break;
        // Serial
        case 3:
            handler = 0x58;
            break;
        // Joypad
        case 4:
            handler = 0x60;
            break;

    }
    cpu->regist->pc = handler;
    tick_m(cpu);
    return 1;
}

//Interrupt utils

int get_if(struct cpu *cpu, int bit)
{
    return (*cpu->_if >> bit) & 0x01;
}

void set_if(struct cpu *cpu, int bit)
{
    *cpu->_if = *cpu->_if | (0x01 << bit);
}

void clear_if(struct cpu *cpu, int bit)
{
    *cpu->_if = *cpu->_if & ~(0x01 << bit);
}

int get_ie(struct cpu *cpu, int bit)
{
    return (*cpu->ie >> bit) & 0x01;
}

void set_ie(struct cpu *cpu, int bit)
{
    *cpu->ie = *cpu->ie | (0x01 << bit);
}

void clear_ie(struct cpu *cpu, int bit)
{
    *cpu->ie = *cpu->ie & ~(0x01 << bit);
}
