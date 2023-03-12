#include <stdlib.h>
#include "cpu.h"

//Rotations, returns 1 if carry else 0
void rotl(uint8_t *src)
{
	*src = (*src << 1) | (*src >> (8 - 1));
}

void rotl_carry(struct cpu_register *regist, uint8_t *src)
{
	rotl(src);
	int a = *src & 1UL; // get first bit
	int b = get_c(regist);
	*src = *src | (1UL & b);
	set_c(regist, a);
}

void rotr(uint8_t *src)
{
	*src = (*src >> 1) | (*src << (8 - 1));
}

void rotl_16(uint16_t *src)
{
	*src = (*src << 1) | (*src >> (16 - 1));
}

void rotr_16(uint16_t *src)
{
	*src = (*src >> 1) | (*src << (16 - 1));
}
