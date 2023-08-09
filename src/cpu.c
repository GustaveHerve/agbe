#include <stdlib.h>
#include "cpu.h"
#include "logic.h"
#include "control.h"
#include "jump.h"
#include "load.h"
#include "rotshift.h"
#include "utils.h"
#include "emulation.h"

#define MEMBUS_SIZE 65536 //In bytes

void cpu_init(struct cpu *cpu)
{
	cpu->regist = malloc(sizeof(struct cpu_register));
	cpu->membus = malloc(sizeof(uint8_t) * MEMBUS_SIZE);
    cpu->ppu = malloc(sizeof(struct ppu));
    ppu_init(cpu->ppu, cpu);
    cpu->ime = 0;
    cpu->ie = &cpu->membus[0xFFFF];
    cpu->_if = &cpu->membus[0xFF0F];
    cpu->div = &cpu->membus[0xFF04];
    cpu->tima = &cpu->membus[0xFF05];
    cpu->tma = &cpu->membus[0xFF06];
    cpu->tac = &cpu->membus[0xFF07];
    cpu->halt = 0;
    cpu->stop = 0;

    *cpu->_if = 0;

    cpu->acc_timer = 0;
}

//Set registers' default values
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
    free(todelete->regist);
	free(todelete);
}

//Interrupt handling
int check_interrupt(struct cpu *cpu)
{
    if (!cpu->ime)
        return 0;

    for (int i = 0; i < 5; i++)
    {
        if (get_if(cpu, i) && get_ie(cpu, i))
            handle_interrupt(cpu, i);
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
    cpu->regist->sp--;
    write_mem(cpu, cpu->regist->sp, hi);
    cpu->regist->sp--;
    write_mem(cpu, cpu->regist->sp, lo);
    uint16_t handler = 0;
    switch (bit)
    {
        //VBlank
        case 0:
            handler = 0x40;
        //LCD STAT
        case 1:
            handler = 0x48;
        //Timer
        case 2:
            handler = 0x50;
        //Serial
        case 3:
            handler = 0x58;
        //Joypad
        case 4:
            handler = 0x60;

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
