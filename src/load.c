#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"


//ld (BC),A
//x02	2 MCycle
int ld_bc_a(struct cpu *cpu)
{
	uint16_t address = 0;
	address = (cpu->regist->b << 8) | cpu->regist->c;
	*(cpu->membus + address) = cpu->regist->a;
	return 2;
}

//ld r,r
//		1 MCycle
int ld_r_r(uint8_t *dest, uint8_t *src))
{
	*dest = *src;
	return 1;
}

//ld r,u8
//x(0-3)(6 or E)	2 MCycle
int ld_r_u8(struct cpu *cpu, uint8_t *dest)
{
	struct cpu_register *regist = cpu->regist;
	uint8_t *mem = cpu->membus;

	cpu->regist->pc++;
	*dest = mem[regist->pc];

	return 2;
}

//ld (HL),u8
//x36
int ld_hl_u8(struct cpu *cpu)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	cpu->regist->pc++;
	cpu->membus[address] = cpu->membus[cpu->regist->pc];
	return 3;
}

//ld u8,r
//x(0-1)A	2 MCycle
int ld_u8_r(struct cpu *cpu, uint8_t *src)
{
	struct cpu_register *regist = cpu->regist;
	uint8_t *mem = cpu->membus;

	cpu->regist->pc++;
	mem[regist->pc] = *src;

	return 2;
}

//ld (HL),r
//		2 MCycle
int ld_hl_r(struct cpu *cpu, uint8_t *src)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	cpu->membus[address] = *src;
	return 2;
}

//ld r,(HL)
//		2 MCycle
int ld_r_hl(struct cpu *cpu, uint8_t *dest)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	uint8_t value = cpu->membus[address];
	*dest = value;
	return 2;
}


//ldi (HL+),A
//x22	2 MCycle
int ldi_hl_a(struct cpu *cpu)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	cpu->membus[address] = cpu->regist->a;

	address++;
	cpu->regist->h = regist_hi(&address);
	cpu->regist->l = regist_lo(&address);
	return 2;
}

//ldd (HL-),A
//x32	2 MCycle
int ldd_hl_a(struct cpu *cpu)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	cpu->membus[address] = cpu->regist->a;

	address--;
	cpu->regist->h = regist_hi(&address);
	cpu->regist->l = regist_lo(&address);
	return 2;
}

//ldi A,(HL+)
//x2A	2 MCycle
int ldi_a_hl(struct cpu *cpu)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	cpu->regist->a = cpu->membus[address];

	address++;
	cpu->regist->h = regist_hi(&address);
	cpu->regist->l = regist_lo(&address);
	return 2;
}

//ldd A,(HL-)
//x3A	2 MCycle
int ldd_a_hl(struct cpu *cpu)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	cpu->regist->a = cpu->membus[address];

	address--;
	cpu->regist->h = regist_hi(&address);
	cpu->regist->l = regist_lo(&address);
	return 2;
}
////
//16 bit operations
////

//ld rr,nn
//x(0-2)1	3 MCycle
int ld_rr_u16(struct cpu *cpu, uint8_t *hi, uint8_t *lo)
{
	struct cpu_register *regist = cpu->regist;
	uint8_t *mem = cpu->membus;

	cpu->regist->pc++;
	*lo = mem[regist->pc];
	cpu->regist->pc++;
	*hi = mem[regist->pc];

	return 3;
}

//ld sp_nn
//x31	3 MCycle
int ld_sp_u16(struct cpu *cpu)
{
	struct cpu_register *regist = cpu->regist;
	uint8_t *mem = cpu->membus;

	cpu->regist->pc++;
	uint8_t lo = mem[regist->pc];
	cpu->regist->pc++;
	uint8_t hi = mem[regist->pc];
	cpu->regist->sp = convert_8to16(&hi, &lo);

	return 3;
}

//ld (nn)_SP
//x08	5 MCycle
int ld_nn_sp(struct cpu *cpu)
{
	cpu->regist->pc++;
	uint8_t *lo = &cpu->membus[cpu->regist->pc];
	cpu->regist->pc++;
	uint8_t *hi = &cpu->membus[cpu->regist->pc];

	uint16_t address = convert_8to16(hi, lo);
	cpu->membus[address] = regist_lo(&cpu->regist->sp);
	cpu->membus[address+1] = regist_hi(&cpu->regist->sp);

	return 5;
}


