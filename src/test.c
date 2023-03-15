#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "utils.h"

int main()
{
	struct cpu *cpu = malloc(sizeof(struct cpu));
	cpu_init(cpu);
	cpu->regist->a = 0x01;
	cpu->regist->f = 0x00;
	cpu->regist->b = 0xF0;
	cpu->regist->c = 0x00;
	cpu->regist->d = 0x01;
	cpu->regist->e = 0xFF;
	cpu->regist->h = 0x00;
	cpu->regist->l = 0x7B;
	cpu->regist->pc = 0x0040;

	cpu->membus[123] = 0xFC;
	rotr_carry(cpu->regist, &cpu->regist->a);

	cpu_free(cpu);
	return 0;
}
