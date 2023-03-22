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

//sto
//x10	1 MCycle
int stop()
{
	//TODO
	return 1;
}

//ccf
//x3F	1 MCycle
int ccf(struct cpu *gb_cpu)
{
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	gb_cpu->regist->f ^= 0x10;
	return 1;
}

//scf
//x37	1 MCycle
int scf(struct cpu *gb_cpu)
{
	set_c(gb_cpu->regist, 1);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	return 1;
}


