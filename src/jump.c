#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

//jr e (signed 8 bit)
//x18	3 MCycle
int jr_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	gb_cpu->regist->pc = gb_cpu->regist->pc + e;

	return 3;
}

//jr nz e (signed 8 bit)
//x20	2 MCycle if condition false, 3 MCycle if condition true
int jr_nz_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	if (!get_z(gb_cpu->regist))
	{
		gb_cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

//jr z e (signed 8 bit)
//x28	2 MCycle if condition false, 3 MCycle if condition true
int jr_z_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	if (get_z(gb_cpu->regist))
	{
		gb_cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

//jr nc e (signed 8 bit)
//x30	2 MCycle if condition false, 3 MCycle if condition true
int jr_nc_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	if (!get_c(gb_cpu->regist))
	{
		gb_cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

//jr c e (signed 8 bit)
//x38	2 MCycle if condition false, 3 MCycle if condition true
int jr_c_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	if (get_c(gb_cpu->regist))
	{
		gb_cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

