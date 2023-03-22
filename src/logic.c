#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

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
	set_n(gb_cpu->regist, 0);
	//TODO H FLAG
	uint16_t address = convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->membus[address]++;
	set_z(gb_cpu->regist, gb_cpu->membus[address] == 0);
	return 3;
}

////inc rr
//x(0-3)3	2 MCycle
int inc_rr(uint8_t *hi, uint8_t *lo)
{
	uint16_t convert = convert_8to16(hi, lo);
	convert += 1;

	*lo = regist_lo(&convert);
	*hi = regist_hi(&convert);

	return 2;
}

//inc SP
//x33	2 MCycle
int inc_sp(uint16_t *dest)
{
	*dest += 1;
	return 2;
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
	set_n(gb_cpu->regist, 1);
	uint16_t address = convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	//TODO HALF CARRY
	gb_cpu->membus[address]--;
	set_z(gb_cpu->regist, gb_cpu->membus[address] == 0);
	return 3;
}

//dec rr
//x(0-2)B	2MCycle
int dec_rr(uint8_t *hi, uint8_t *lo)
{
	uint16_t temp = convert_8to16(hi, lo);
	temp--;

	*lo = regist_lo(&temp);
	*hi = regist_hi(&temp);
	return 2;
}

//dec sp
//x3B	2 MCycle
int dec_sp(uint16_t *sp)
{
	*sp -= 1;
	return 2;
}



// add HL,rr
// x(0-2)9	2 MCycle
int add_hl_rr(struct cpu *gb_cpu, uint8_t *hi, uint8_t *lo)
{
	//TODO C FLAG
	set_n(gb_cpu->regist, 0);
	hflag16_add_set(gb_cpu->regist, convert_8to16(&gb_cpu->regist->h,
				&gb_cpu->regist->l), convert_8to16(hi, lo));
	uint16_t sum = convert_8to16(hi, lo) +
		convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->regist->h = regist_hi(&sum);
	gb_cpu->regist->l = regist_lo(&sum);
	return 2;
}

// add HL,SP
// x39	2 MCycle
int add_hl_sp(struct cpu *gb_cpu)
{
	//TODO C FLAG
	set_n(gb_cpu->regist, 0);
	hflag16_add_set(gb_cpu->regist, convert_8to16(&gb_cpu->regist->h,
				&gb_cpu->regist->l), gb_cpu->regist->sp);
	uint16_t sum = gb_cpu->regist->sp +
		convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->regist->h = regist_hi(&sum);
	gb_cpu->regist->l = regist_lo(&sum);
	return 2;
}


//daa A
//x27	1 MCycle
int daa(struct cpu *gb_cpu)
{
	uint8_t *a = &gb_cpu->regist->a;
	if (!get_n(gb_cpu->regist))	//Additioncase
	{
		if (get_c(gb_cpu->regist) || *a > 0x99) //check high nibble
		{
			*a += 0x60;
			set_c(gb_cpu->regist, 1);
		}

		if (get_h(gb_cpu->regist)|| (*a & 0x0F) > 0x09)	//check low nibble
		{
			*a += 0x06;
		}
	}

	else	//Subtraction case
	{
		if (get_c(gb_cpu->regist))
			*a -= 0x60;
		if (get_h(gb_cpu->regist))
			*a -= 0x06;
	}

	return 1;
}



//cpl
//x2F	1 MCycle
int cpl(struct cpu *gb_cpu)
{
	gb_cpu->regist->a = ~gb_cpu->regist->a;
	set_n(gb_cpu->regist, 1);
	set_h(gb_cpu->regist, 1);
	return 1;
}
