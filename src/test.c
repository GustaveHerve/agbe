#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"

int main()
{
	struct cpu *cpu = malloc(sizeof(struct cpu));
	cpu_init(cpu);
	cpu->regist->b = 0x00;
	cpu->regist->c = 0x00;
	cpu->regist->d = 0x01;
	cpu->regist->e = 0xFF;
	cpu->regist->pc = 0x0040;
	cpu->membus[65] = 0x34;
	cpu->membus[66] = 0x23;

	ld_16bit(cpu, &cpu->regist->b, &cpu->regist->c);
	inc_16bit(&cpu->regist->d, &cpu->regist->e);
	inc_16bit(&cpu->regist->d, &cpu->regist->e);
	inc_16bit(&cpu->regist->d, &cpu->regist->e);

	cpu_free(cpu);
	return 0;
}
