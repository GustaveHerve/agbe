#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

//jr e (signed 8 bit)
//x18	3 MCycle
int jr_e8(struct cpu *cpu)
{
	cpu->regist->pc++;
	int8_t e = cpu->membus[cpu->regist->pc];
	cpu->regist->pc = cpu->regist->pc + e;

	return 3;
}

//jr cc e (signed 8 bit)
int jr_cc_e8(struct cpu *cpu, int cc)
{
    cpu->regist->pc++;
    int8_t e = cpu->membus[cpu->regist->pc];
    if (cc)
    {
        cpu->regist->pc += e;
        return 3;
    }
    return 2;
}
/*
//jr nz e (signed 8 bit)
//x20	2 MCycle if condition false, 3 MCycle if condition true
int jr_nz_e(struct cpu *cpu)
{
	cpu->regist->pc++;
	int8_t e = cpu->membus[cpu->regist->pc];
	if (!get_z(cpu->regist))
	{
		cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

//jr z e (signed 8 bit)
//x28	2 MCycle if condition false, 3 MCycle if condition true
int jr_z_e(struct cpu *cpu)
{
	cpu->regist->pc++;
	int8_t e = cpu->membus[cpu->regist->pc];
	if (get_z(cpu->regist))
	{
		cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

//jr nc e (signed 8 bit)
//x30	2 MCycle if condition false, 3 MCycle if condition true
int jr_nc_e(struct cpu *cpu)
{
	cpu->regist->pc++;
	int8_t e = cpu->membus[cpu->regist->pc];
	if (!get_c(cpu->regist))
	{
		cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

//jr c e (signed 8 bit)
//x38	2 MCycle if condition false, 3 MCycle if condition true
int jr_c_e(struct cpu *cpu)
{
	cpu->regist->pc++;
	int8_t e = cpu->membus[cpu->regist->pc];
	if (get_c(cpu->regist))
	{
		cpu->regist->pc += e;
		return 3;
	}

	return 2;
}
*/

//ret
//xC9   4 MCycles
int ret(struct cpu *cpu)
{
    uint8_t lo = cpu->membus[cpu->regist->sp];
    cpu->regist->sp += 1;
    uint8_t hi  = cpu->membus[cpu->regist->sp];
    cpu->regist->sp += 1;
    cpu->regist->pc = convert_8to16(&hi, &lo);
    return 4;
}

//ret cc
//
int ret_cc(struct cpu *cpu, int cc)
{
    if (cc)
    {
        uint8_t lo = cpu->membus[cpu->regist->sp];
        cpu->regist->sp += 1;
        uint8_t hi = cpu->membus[cpu->regist->sp];
        cpu->regist->sp += 1;
        cpu->regist->pc = convert_8to16(&hi, &lo);
        return 5;
    }
    return 2;
}

int jp_hl(struct cpu *cpu)
{
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    cpu->regist->pc = address;
    return 1;
}

int jp_nn(struct cpu *cpu)
{
    cpu->regist->pc++;
    uint8_t lo = cpu->membus[cpu->regist->pc];
    cpu->regist->pc++;
    uint8_t hi = cpu->membus[cpu->regist->pc];
    uint16_t address = convert_8to16(&hi, &lo);
    cpu->regist->pc = address;
    return  4;
}
