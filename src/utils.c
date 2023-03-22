#include <stdlib.h>
#include "cpu.h"

//8 bit and 16 bit manipulations
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

uint8_t get_lsb_nibble(uint8_t b)
{
	return b & 0xF;
}

uint8_t get_msb_nibble(uint8_t b)
{
	return (b >> 4) & 0xF;
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
	return (regist->f >> 7) & 1UL;
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
	return (regist->f >> 6) & 1UL;
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
	return (regist->f >> 5) & 1UL;
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
	return (regist->f >> 4) & 1UL;
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

//TODO Probably wrong Complementary addition instead ?
int hflag_sub_check(uint8_t a, uint8_t b)
{
	return hflag_check(get_lsb_nibble(a) - get_lsb_nibble(b));
}

void hflag_sub_set(struct cpu_register *regist, uint8_t a, uint8_t b)
{
	set_h(regist, hflag_sub_check(a, b));
}

int hflag16_check(uint16_t result)
{
	return (result & 0x1000) == 0x1000;
}

int hflag16_add_check(uint16_t a, uint16_t b)
{
	return hflag16_check(a & 0xFFF + b & 0xFFF);
}

void hflag16_add_set(struct cpu_register *regist, uint16_t a, uint16_t b)
{
	set_h(regist, hflag16_add_check(a, b));
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

int cflag_add_check(uint8_t a, uint8_t b)
{
	return ((a + b) & 0x100) == 0x100;
}

void cflag_add_set(struct cpu_register *regist, uint8_t a, uint8_t b)
{
	set_c(regist, cflag_add_check(a, b));
}

int cflag_sub_check(uint8_t a, uint8_t b)
{
	return ((a - b) & 0x100) == 0x100;
}

void cflag_sub_set(struct cpu_register *regist, uint8_t a, uint8_t b)
{
	set_c(regist, cflag_sub_check(a, b));
}


//Rotations, returns 1 if carry else 0
void rotl(uint8_t *src)
{
	*src = (*src << 1) | (*src >> (8 - 1));
}

void rotl_carry(struct cpu_register *regist, uint8_t *src)
{
	rotl(src);
	int a = *src & 1UL; // get lsb
	int b = get_c(regist);
	if (b)
		*src = *src | 1UL;
	else
		*src = *src & ~(1UL);
	set_c(regist, a);
}

void rotr(uint8_t *src)
{
	*src = (*src >> 1) | (*src << (8 - 1));
}

void rotr_carry(struct cpu_register *regist, uint8_t *src)
{
	rotr(src);
	int a = (*src >> 7) & 1UL; // get msb
	int b = get_c(regist);
	if (b)
		*src = *src | (1UL << 7);
	else
		*src = *src & ~(1UL << 7);

	set_c(regist, a);
}

void rotl_16(uint16_t *src)
{
	*src = (*src << 1) | (*src >> (16 - 1));
}

void rotr_16(uint16_t *src)
{
	*src = (*src >> 1) | (*src << (16 - 1));
}
