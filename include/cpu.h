#ifndef CPU_H
#define CPU_H

struct cpu_register
{
	uint16_t af;
	uint16_t bc;
	uint16_t de;
	uint16_t hl;
	uint16_t sp;
	uint16_t pc;
};

struct cpu
{
	struct cpu_register regist;
	uint8_t *membus;
};

int nop(uint16_t *pc);
int ld_16bit(uint16_t *pc, int opcode);

#endif
