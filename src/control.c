#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

//nop
//x00	1 MCycle
int nop()
{
	return 1;
}

//stop
//x10	1 MCycle
int stop(struct cpu *cpu)
{
    cpu->stop = 1;
	return 1;
}

//halt
int halt(struct cpu *cpu)
{
    cpu->halt = 1;
	return 1;
}

//ccf
//x3F	1 MCycle
int ccf(struct cpu *cpu)
{
	set_n(cpu->regist, 0);
	set_h(cpu->regist, 0);
	cpu->regist->f ^= 0x10;
	return 1;
}

//scf
//x37	1 MCycle
int scf(struct cpu *cpu)
{
	set_c(cpu->regist, 1);
	set_n(cpu->regist, 0);
	set_h(cpu->regist, 0);
	return 1;
}

int di(struct cpu *cpu)
{
    cpu->membus[0xFFFF] = 0;
    return 1;
}

int ei(struct cpu *cpu)
{
    cpu->ime_enable = 2;
    return 1;
}
