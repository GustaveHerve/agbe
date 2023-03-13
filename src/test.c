#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "utils.h"

int main()
{
	struct cpu *cpu = malloc(sizeof(struct cpu));
	cpu_init(cpu);
	cpu->regist->a = 0x80;
	cpu->regist->b = 0xF0;
	cpu->regist->c = 0x00;
	cpu->regist->d = 0x01;
	cpu->regist->e = 0xFF;
	cpu->regist->h = 0x01;
	cpu->regist->l = 0x90;
	cpu->regist->pc = 0x0040;
	cpu->membus[65] = 0x34;
	cpu->membus[66] = 0x23;
	cpu->membus[400] = 0x02;

	inc_hl(cpu);
	dec_hl(cpu);

	ld_hl_u8(cpu);
	rlca(cpu);
	rlca(cpu);

	cpu_free(cpu);
	return 0;
}
