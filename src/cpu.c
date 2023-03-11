#include <stdlib.h>
#include <err.h>
#include "cpu.h"

#define MEMBUS_SIZE 65536 //In bytes

void cpu_init(struct cpu *new_cpu)
{
	new_cpu->regist = malloc(sizeof(struct cpu_register));
	new_cpu->membus = malloc(sizeof(uint8_t) * MEMBUS_SIZE);
}

void cpu_free(struct cpu *todelete)
{
	free(todelete->membus);
	free(todelete);
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

uint8_t get_lsb_nibble(uint8_t byte)
{
	return byte & 0xF;
}

uint8_t get_msb_nibble(uint8_t byte)
{
	return (byte >> 4) & 0xF;
}

uint16_t convert_8to16(uint8_t hi, uint8_t lo)
{
	uint16_t res = 0;
	res = (res | hi) << 8;
	res = res | lo;
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

int hflag_check(uint8_t result)
{
	return (result & 0x10) == 0x10;
}

int hflag_add_check(uint8_t a, uint8_t b)
{
	return hflag_check(get_lsb_nibble(a) + get_lsb_nibble(b));
}

void hflag_add_set(struct cpu_register *regist, uint8_t a, uint8_t b)
{
	set_c(regist, hflag_add_check(a, b));
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
int ld_16bit(struct cpu *gb_cpu, uint16_t *dest)
{
	struct cpu_register *regist = gb_cpu->regist;
	uint8_t *mem = gb_cpu->membus;

	gb_cpu->regist->pc++;
	*dest = (*dest & 0xFF00) | mem[regist->pc];
	gb_cpu->regist->pc++;
	*dest = (*dest & 0x00FF) | (mem[regist->pc] << 8);

	//uint16_t nn = (mem[regist->++pc] & 0xFF) |
	//   				((mem[regist->pc] & 0xFF) << 8);
	//*dest = nn;
	return 3;
}

//ld (BC),A
//x02	2 MCycle
int ld_bc_a(struct cpu *gb_cpu)
{
	uint16_t address = gb_cpu->regist->bc;
	*(gb_cpu->membus + address) = regist_hi(&gb_cpu->regist->af);
	return 2;
}

//inc rr (16 bit)
//x(0-3)3	2 MCycle
int inc_16bit(uint16_t *dest)
{
	*dest += 1;
	return 2;
}

//inc r (8 bit)
//x(0-3)(4 or C)	1 MCycle
int inc_r(struct cpu *gb_cpu, uint8_t *dest)
{
	set_n(gb_cpu->regist, 0);
	set_z(gb_cpu->regist, *dest == 0);
	hflag_add_set(gb_cpu->regist, *dest, 1);

	*dest += 1;
	return 1;
}
/*
//inc hl (8 bit)
//x34	3 MCycle
int inc_hl(struct cpu *gb_cpu)
{
	return 3;
}
*/
