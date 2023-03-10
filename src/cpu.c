#include <stdlib.h>
#include <err.h>

#define MEMBUS_SIZE 65536 //In bytes

void cpu_init(struct *cpu new_cpu)
{
	new_cpu->regist = malloc(sizeof(struct cpu_register));
	new_cpu->membus = malloc(sizeof(uint8_t) * MEMBUS_SIZE);
	return;
}

uint8_t regist_hi(uint16_t *rr)
{
	uint8_t res = (*rr >> 8) & 0xFF;
	return res;
}

uint8_t regist_lo(uint16_t *rr)
{
	uint8_t res = *rr & 0xFF;
	return res;
}

///////////////
//Flags methods
///////////////

void set_z(struct cpu_register *regist, int value)
{
	if (value)
		regist->af = regist->af | 1UL << 7;
	else
		regist->af = regist->af & ~(1UL << 7);
}

void set_n(struct cpu_register *regist, int value)
{
	if (value)
		regist->af = regist->af | 1UL << 6;
	else
		regist->af = regist->af & ~(1UL << 6);
}

void set_h(struct cpu_register *regist, int value)
{
	if (value)
		regist->af = regist->af | 1UL << 5;
	else
		regist->af = regist->af & ~(1UL << 5);
}

void set_c(struct cpu_register *regist, int value)
{
	if (value)
		regist->af = regist->af | 1UL << 4;
	else
		regist->af = regist->af & ~(1UL << 4);
}


/////////////////
//Instruction set
/////////////////

//nop
//x00	1 MCycle
int nop()
{
	return 1;
}

//ld rr,nn (16-bit)
//x(0-3)1	3 MCycle
int ld_16bit(struct cpu *gb_cpu, int opcode)
{
	int *rr = 0;;
	struct cpu_register *regist = gb_cpu->regist;
	uint8_t mem = gb_cpu->membus;
	switch (opcode)
	{
		case 0x01:
			rr = regist->&bc;
			break;
		case 0x11:
			rr = regist->&de;
			break;
		case 0x21:
			rr = regist->&hl;
			break;
		case 0x31:
			rr = regist->&sp;
			break;

		default:
			errx(-1, "ld_16bit: invalid opcode given!");
	}

	uint16_t nn = (mem[regist->++pc] & 0xFF) |
	   				((mem[regist->pc] & 0xFF) << 8);
	*rr = nn;
	return 3;
}

//ld (BC),A
//x02	2 MCycle
int ld_bc_a(struct cpu *gb_cpu)
{
	uint8_t address = gb_cpu->regist->bc;
	gb_cpu->*(membus + addres) = regist_hi(gb_cpu->regist->af);
	return 2;
}

//inc rr (16 bit)
//x(0-3)3	2 MCycle
int inc_16bit(struct cpu_register *regist, int opcode)
{
	uint16_t *to_incr = 0;
	switch(opcode)
	{
		case 0x03:
			to_incr = regist->&bc;
			break;
		case 0x13:
			to_incr = regist->&de;
			break;
		case 0x23:
			to_incr = regist->&hl;
			break;
		case 0x33:
			to_incr = regist->&sp;
			break;

		default:
			errx(-1, "inc_16bit: invalid opcode given!");
	}

	*to_incr++;
	return 2;
}

//inc r (8 bit)
//x(0-3)4	1 MCycle
int inc_r(struct cpu *gb_cpu, int opcode)
{
	switch(opcode)
	{
		case 0x04:


	}
	set_n(gb_cpu->&regist, 0);

	return 1;
}
