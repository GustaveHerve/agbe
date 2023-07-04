#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "utils.h"
#include "logic.h"
#include "prefix.h"

int main()
{
	struct cpu *cpu = malloc(sizeof(struct cpu));
	cpu_init(cpu);
	cpu->regist->a = 0x0A;
	cpu->regist->f = 0xF4;
	cpu->regist->b = 0xF0;
	cpu->regist->c = 0x00;
	cpu->regist->d = 0x01;
	cpu->regist->e = 0xFF;
	cpu->regist->h = 0x00;
	cpu->regist->l = 0x7B;
	cpu->regist->pc = 0x09;
    cpu->regist->sp = 0x0A;
    cpu->membus[10] = 251;
    swap(&cpu->regist->f);

	cpu_free(cpu);
	return 0;
}
