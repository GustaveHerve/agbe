#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

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

uint16_t convert_8to16(uint8_t *hi, uint8_t *lo)
{
	uint16_t res = 0;
	res = (res | *hi) << 8;
	res = res | *lo;
	return res;
}


///////////////
//Flags methods
///////////////

int get_z(struct cpu_register *regist)
{
	return (regist->f >> 7) | 1UL;
}

void set_z(struct cpu_register *regist, int value)
{
	if (value)
		regist->f = regist->f | 1UL << 7;
	else
		regist->f = regist->f & ~(1UL << 7);
}

int get_n(struct cpu_register *regist)
{
	return (regist->f >> 6) | 1UL;
}

void set_n(struct cpu_register *regist, int value)
{
	if (value)
		regist->f = regist->f | 1UL << 6;
	else
		regist->f = regist->f & ~(1UL << 6);
}

int get_h(struct cpu_register *regist)
{
	return (regist->f >> 5) | 1UL;
}

void set_h(struct cpu_register *regist, int value)
{
	if (value)
		regist->f = regist->f | 1UL << 5;
	else
		regist->f = regist->f & ~(1UL << 5);
}

int get_c(struct cpu_register *regist)
{
	return (regist->f >> 4) | 1UL;
}

void set_c(struct cpu_register *regist, int value)
{
	if (value)
		regist->f = regist->f | 1UL << 4;
	else
		regist->f = regist->f & ~(1UL << 4);
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
	set_h(regist, hflag_add_check(a, b));
}

int hflag_sub_check(uint8_t a, uint8_t b)
{
	return hflag_check(get_lsb_nibble(a) - get_lsb_nibble(b));
}

void hflag_sub_set(struct cpu_register *regist, uint8_t a, uint8_t b)
{
	set_h(regist, hflag_sub_check(a, b));
}

int cflag_rotl_check(uint8_t src)
{
	return (src & 0x01) == 0x01;
}

void cflag_rotl_set(struct cpu_register *regist, uint8_t src)
{
	set_c(regist, cflag_rotl_check(src));
}

int cflag_rotr_check(uint8_t src)
{
	return (src & 0x80) == 0x80;
}

void cflag_rotr_set(struct cpu_register *regist, uint8_t src)
{
	set_c(regist, cflag_rotr_check(src));
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

//ld (BC),A
//x02	2 MCycle
int ld_bc_a(struct cpu *gb_cpu)
{
	uint16_t address = 0;
	address = (gb_cpu->regist->b << 8) | gb_cpu->regist->c;
	*(gb_cpu->membus + address) = gb_cpu->regist->a;
	return 2;
}

//inc r (8 bit)
//x(0-3)(4 or C)	1 MCycle
int inc_r(struct cpu *gb_cpu, uint8_t *dest)
{
	set_n(gb_cpu->regist, 0);
	hflag_add_set(gb_cpu->regist, *dest, 1);

	*dest += 1;
	set_z(gb_cpu->regist, *dest == 0);
	return 1;
}

//inc (HL) (8 bit)
//x34	3 MCycle
int inc_hl(struct cpu *gb_cpu)
{
	uint16_t address = convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->membus[address]++;
	return 3;
}

//dec r (8 bit)
//x(0-3)(5 or D)	1 MCycle
int dec_r(struct cpu *gb_cpu, uint8_t *dest)
{
	set_n(gb_cpu->regist, 1);
	hflag_sub_set(gb_cpu->regist, *dest, 1);

	*dest -= 1;
	set_z(gb_cpu->regist, *dest == 0);
	return 1;
}

//dec (HL) (8 bit)
//x35	3 MCycle
int dec_hl(struct cpu *gb_cpu)
{
	uint16_t address = convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->membus[address]--;
	return 3;
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

////
//16 bit operations
////

//ld rr,nn (16-bit)
//x(0-3)1	3 MCycle
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

////inc rr (16 bit)
//x(0-3)3	2 MCycle
int inc_rr(uint8_t *hi, uint8_t *lo)
{
	uint16_t convert = convert_8to16(hi, lo);
	convert += 1;

	*lo = regist_lo(&convert);
	*hi = regist_hi(&convert);

	return 2;
}

//inc rr_SP
//
int inc_rr_sp(uint16_t *dest)
{
	*dest += 1;
	return 2;
}

// add HL,rr
// x(0-2)9	2 MCycle
int add_hl_rr(struct cpu *gb_cpu, uint8_t *hi, uint8_t *lo)
{
	set_n(gb_cpu->regist, 0);
	hflag_add_set(gb_cpu->regist, gb_cpu->regist->l, *lo);

	uint16_t sum = convert_8to16(hi, lo) +
		convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->regist->h = regist_hi(&sum);
	gb_cpu->regist->l = regist_lo(&sum);
	return 2;
}

/////
//Shift and Rotations
/////

//rlc A
//x07	1 MCycle
int rlca(struct cpu *gb_cpu)
{
	rotl(&gb_cpu->regist->a);
	set_z(gb_cpu->regist, 0);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	cflag_rotl_set(gb_cpu->regist, gb_cpu->regist->a);
	return 1;
}


//rla A
//x17	1 MCycle
int rla(struct cpu *gb_cpu)
{
	rotl_carry(gb_cpu->regist, &gb_cpu->regist->a);
	set_z(gb_cpu->regist, 0);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	return 1;
}
