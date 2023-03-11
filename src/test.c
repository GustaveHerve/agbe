#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"

int main()
{
	struct cpu *cpu = malloc(sizeof(struct cpu));
	cpu_init(cpu);
	cpu->regist->bc = 0x0000;
	cpu->regist->de = 0xFFFF;
	cpu->regist->pc = 0x0040;
	cpu->membus[65] = 0x34;
	cpu->membus[66] = 0x23;

	ld_16bit(cpu, &cpu->regist->bc);
	inc_16bit(&cpu->regist->de);
	uint8_t *test = (uint8_t*)&cpu->regist->pc;

	cpu_free(cpu);
	return 0;
}
