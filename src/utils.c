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
