#include <stdlib.h>
#include "cpu.h"
#include "logic.h"
#include "control.h"
#include "jump.h"
#include "load.h"
#include "rotshift.h"
#include "utils.h"

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

void next_op(struct cpu *cpu)
{
    int mcycles = 0;
    switch (cpu->membus[cpu->regist->pc])
    {
        case 0x00:
            mcycles = nop();
            break;
        case 0x01:
            mcycles = ld_rr_nn(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x02:
            mcycles = ld_rr_a(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x03:
            mcycles = inc_rr(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x04:
            mcycles = inc_r(cpu, &cpu->regist->b);
            break;
        case 0x05:
            mcycles = dec_r(cpu, &cpu->regist->b);
            break;
        case 0x06:
            mcycles = ld_r_u8(cpu, &cpu->regist->b);
            break;
        case 0x07:
            mcycles = rlca(cpu);
            break;
        case 0x08:
            mcycles = ld_nn_sp(cpu);
            break;
        case 0x09:
            mcycles = add_hl_rr(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x0A:
            mcycles = ld_rr_nn(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x0B:
            mcycles = dec_rr(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x0C:
            mcycles = inc_r(cpu, &cpu->regist->c);
            break;
        case 0x0D:
            mcycles = dec_r(cpu, &cpu->regist->c);
            break;
        case 0x0E:
            mcycles = ld_r_u8(cpu, &cpu->regist->c);
            break;
        case 0x0F:
            rrca(cpu);
            break;
        case 0x10:
            stop(cpu);
            break;
        case 0x11:
            ld_rr_nn(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x12:



    }
}

//Interrupt utils
void set_if(struct cpu *cpu, int bit)
{
    *cpu->_if = *cpu->_if | (0x01 << bit);
}

void clear_if(struct cpu *cpu, int bit)
{
    *cpu->_if = *cpu->_if & ~(0x01 << bit);
}
