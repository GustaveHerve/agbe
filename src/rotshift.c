#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

//rlca A
//x07	1 MCycle
int rlca(struct cpu *gb_cpu)
{
	rotl(&gb_cpu->regist->a);
	set_z(gb_cpu->regist, 0);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	cflag_rotl_set(gb_cpu->regist, gb_cpu->regist->a);
	return 1;
}

//rla A
//x17	1 MCycle
int rla(struct cpu *gb_cpu)
{
	rotl_carry(gb_cpu->regist, &gb_cpu->regist->a);
	set_z(gb_cpu->regist, 0);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	return 1;
}

//rrca A
//x0F	1 MCycle
int rrca(struct cpu *gb_cpu)
{
	rotr(&gb_cpu->regist->a);
	set_z(gb_cpu->regist, 0);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	cflag_rotr_set(gb_cpu->regist, gb_cpu->regist->a);
	return 1;
}

//rra A
//x1F	1 MCycle
int rra(struct cpu *gb_cpu)
{
	rotr_carry(gb_cpu->regist, &gb_cpu->regist->a);
	set_z(gb_cpu->regist, 0);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	return 1;
}
