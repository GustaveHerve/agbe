#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

//jr e (signed 8 bit)
//x18	3 MCycle
int jr_e(struct cpu *cpu)
{
	cpu->regist->pc++;
	int8_t e = cpu->membus[cpu->regist->pc];
	cpu->regist->pc = cpu->regist->pc + e;

	return 3;
}

//jr nz e (signed 8 bit)
//x20	2 MCycle if condition false, 3 MCycle if condition true
int jr_nz_e(struct cpu *cpu)
{
	cpu->regist->pc++;
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
	cpu->regist->pc++;
	int8_t e = cpu->membus[cpu->regist->pc];
	if (get_c(cpu->regist))
	{
		cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

