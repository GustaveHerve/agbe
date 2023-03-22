#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

#define MEMBUS_SIZE 65536 //In bytes

void cpu_init(struct cpu *cpu)
{
	cpu->regist = malloc(sizeof(struct cpu_register));
	cpu->membus = malloc(sizeof(uint8_t) * MEMBUS_SIZE);
}

void cpu_free(struct cpu *todelete)
{
	free(todelete->membus);
	free(todelete);
}
