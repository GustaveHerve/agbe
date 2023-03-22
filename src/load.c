#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"


//ld (BC),A
//x02	2 MCycle
int ld_bc_a(struct cpu *gb_cpu)
{
	uint16_t address = 0;
	address = (gb_cpu->regist->b << 8) | gb_cpu->regist->c;
	*(gb_cpu->membus + address) = gb_cpu->regist->a;
	return 2;
}

//ld r,u8
//x(0-3)(6 or E)	2 MCycle
int ld_r_u8(struct cpu *gb_cpu, uint8_t *dest)
{
	struct cpu_register *regist = gb_cpu->regist;
	uint8_t *mem = gb_cpu->membus;

	gb_cpu->regist->pc++;
	*dest = mem[regist->pc];

	return 2;
}

//ld (HL),u8
//x36
int ld_hl_u8(struct cpu *gb_cpu)
{
	uint16_t address = convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->regist->pc++;
	gb_cpu->membus[address] = gb_cpu->membus[gb_cpu->regist->pc];
	return 3;
}

//ld u8,r
//x(0-1)A	2 MCycle
int ld_u8_r(struct cpu *gb_cpu, uint8_t *src)
{
	struct cpu_register *regist = gb_cpu->regist;
	uint8_t *mem = gb_cpu->membus;

	gb_cpu->regist->pc++;
	mem[regist->pc] = *src;

	return 2;
}



//ldi (HL+),A
//x22	2 MCycle
int ldi_hl_a(struct cpu *gb_cpu)
{
	uint16_t address = convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->membus[address] = gb_cpu->regist->a;

	address++;
	gb_cpu->regist->h = regist_hi(&address);
	gb_cpu->regist->l = regist_lo(&address);
	return 2;
}

//ldd (HL-),A
//x32	2 MCycle
int ldd_hl_a(struct cpu *gb_cpu)
{
	uint16_t address = convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->membus[address] = gb_cpu->regist->a;

	address--;
	gb_cpu->regist->h = regist_hi(&address);
	gb_cpu->regist->l = regist_lo(&address);
	return 2;
}

//ldi A,(HL+)
//x2A	2 MCycle
int ldi_a_hl(struct cpu *gb_cpu)
{
	uint16_t address = convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->regist->a = gb_cpu->membus[address];

	address++;
	gb_cpu->regist->h = regist_hi(&address);
	gb_cpu->regist->l = regist_lo(&address);
	return 2;
}

//ldd A,(HL-)
//x3A	2 MCycle
int ldd_a_hl(struct cpu *gb_cpu)
{
	uint16_t address = convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->regist->a = gb_cpu->membus[address];

	address--;
	gb_cpu->regist->h = regist_hi(&address);
	gb_cpu->regist->l = regist_lo(&address);
	return 2;
}
////
//16 bit operations
////

//ld rr,nn
//x(0-2)1	3 MCycle
int ld_rr_u16(struct cpu *gb_cpu, uint8_t *hi, uint8_t *lo)
{
	struct cpu_register *regist = gb_cpu->regist;
	uint8_t *mem = gb_cpu->membus;

	gb_cpu->regist->pc++;
	*lo = mem[regist->pc];
	gb_cpu->regist->pc++;
	*hi = mem[regist->pc];

	return 3;
}

//ld sp_nn
//x31	3 MCycle
int ld_sp_u16(struct cpu *gb_cpu)
{
	struct cpu_register *regist = gb_cpu->regist;
	uint8_t *mem = gb_cpu->membus;

	gb_cpu->regist->pc++;
	uint8_t lo = mem[regist->pc];
	gb_cpu->regist->pc++;
	uint8_t hi = mem[regist->pc];
	gb_cpu->regist->sp = convert_8to16(&hi, &lo);

	return 3;
}

//ld (nn)_SP
//x08	5 MCycle
int ld_nn_sp(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	uint8_t *lo = &gb_cpu->membus[gb_cpu->regist->pc];
	gb_cpu->regist->pc++;
	uint8_t *hi = &gb_cpu->membus[gb_cpu->regist->pc];

	uint16_t address = convert_8to16(hi, lo);
	gb_cpu->membus[address] = regist_lo(&gb_cpu->regist->sp);
	gb_cpu->membus[address+1] = regist_hi(&gb_cpu->regist->sp);

	return 5;
}


