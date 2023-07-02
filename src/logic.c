#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

//inc r (8 bit)
//x(0-3)(4 or C)	1 MCycle
int inc_r(struct cpu *cpu, uint8_t *dest)
{
	set_n(cpu->regist, 0);
	hflag_add_set(cpu->regist, *dest, 1);

	*dest += 1;
	set_z(cpu->regist, *dest == 0);
	return 1;
}

//inc (HL) (8 bit)
//x34	3 MCycle
int inc_hl(struct cpu *cpu)
{
	set_n(cpu->regist, 0);
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	uint8_t value = cpu->membus[address];
	hflag_add_set(cpu->regist, value, 1);
	cpu->membus[address]++;
	set_z(cpu->regist, cpu->membus[address] == 0);
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
int dec_r(struct cpu *cpu, uint8_t *dest)
{
	set_n(cpu->regist, 1);
	hflag_sub_set(cpu->regist, *dest, 1);

	*dest -= 1;
	set_z(cpu->regist, *dest == 0);
	return 1;
}

//dec (HL) (8 bit)
//x35	3 MCycle
int dec_hl(struct cpu *cpu)
{
	set_n(cpu->regist, 1);
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    hflag_sub_set(cpu->regist, cpu->membus[address], 1);
	cpu->membus[address]--;
	set_z(cpu->regist, cpu->membus[address] == 0);
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

//add A,r
//x     1 MCycle
int add_a_r(struct cpu *cpu, uint8_t *src)
{
    set_n(cpu->regist, 0);
    hflag_add_set(cpu->regist, cpu->regist->a, *src);
    cflag_add_set(cpu->regist, cpu->regist->a, *src);
    cpu->regist->a += *src;
    set_z(cpu->regist, cpu->regist->a == 0);
    return 1;
}

//add A,(HL)
//x     2 MCycle
int add_a_hl(struct cpu *cpu)
{
    set_n(cpu->regist, 0);
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    uint8_t val = cpu->membus[address];
    hflag_add_set(cpu->regist, cpu->regist->a, val);
    cflag_add_set(cpu->regist, cpu->regist->a, val);
    cpu->regist->a += val;
    set_z(cpu->regist, cpu->regist->a == 0);
    return 2;
}

//adc A,r
//x     1 MCycle
int adc_a_r(struct cpu *cpu, uint8_t *src)
{
    set_n(cpu->regist, 0);
    hflag_add_set(cpu->regist, cpu->regist->a, *src);
    cflag_add_set(cpu->regist, cpu->regist->a, *src);
    cpu->regist->a += *src;
    if (!get_h(cpu->regist))
        hflag_add_set(cpu->regist, cpu->regist->a, 1);
    if (!get_c(cpu->regist))
        cflag_add_set(cpu->regist, cpu->regist->a, 1);
    cpu->regist->a++;
    set_z(cpu->regist, cpu->regist->a == 0);
    return 1;
}

//adc A,(HL)
//x     2 MCycle
int adc_a_hl(struct cpu *cpu)
{
    set_n(cpu->regist, 0);
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    uint8_t val = cpu->membus[address];
    hflag_add_set(cpu->regist, cpu->regist->a, val);
    cflag_add_set(cpu->regist, cpu->regist->a, val);
    cpu->regist->a += val;
    if (!get_h(cpu->regist))
        hflag_add_set(cpu->regist, cpu->regist->a, 1);
    if (!get_c(cpu->regist))
        cflag_add_set(cpu->regist, cpu->regist->a, 1);
    cpu->regist->a++;
    set_z(cpu->regist, cpu->regist->a == 0);
    return 2;
}

// add HL,rr
// x(0-2)9	2 MCycle
int add_hl_rr(struct cpu *cpu, uint8_t *hi, uint8_t *lo)
{
	set_n(cpu->regist, 0);
    uint16_t hl = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    uint16_t rr = convert_8to16(hi, lo);
	hflag16_add_set(cpu->regist, hl, rr);
    cflag16_add_set(cpu->regist, hl, rr);
	uint16_t sum = hl + rr;
	cpu->regist->h = regist_hi(&sum);
	cpu->regist->l = regist_lo(&sum);
	return 2;
}

// add HL,SP
// x39	2 MCycle
int add_hl_sp(struct cpu *cpu)
{
	set_n(cpu->regist, 0);
    uint16_t hl = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	hflag16_add_set(cpu->regist, hl, cpu->regist->sp);
    cflag16_add_set(cpu->regist, hl, cpu->regist->sp);
	uint16_t sum = cpu->regist->sp + hl;
	cpu->regist->h = regist_hi(&sum);
	cpu->regist->l = regist_lo(&sum);
	return 2;
}

int add_sp_e8(struct cpu *cpu)
{
    cpu->regist->pc++;
    uint8_t offset = cpu->membus[cpu->regist->pc];
    uint8_t p = regist_lo(&cpu->regist->sp);
    hflag_add_set(cpu->regist, p, offset);
    cflag_add_set(cpu->regist, p, offset);
    cpu->regist->sp += offset;
    return 4;
}
//sub A,r
//x(0-7)9   1 MCycle
int sub_a_r(struct cpu *cpu, uint8_t *src)
{
    set_n(cpu->regist, 1);
    cflag_sub_set(cpu->regist, cpu->regist->a, *src);
    hflag_sub_set(cpu->regist, cpu->regist->a, *src);
    cpu->regist->a -= *src;
    set_z(cpu->regist, cpu->regist->a == 0);
    return 1;
}

//sub A,(HL)
//x69   2 MCycle
int sub_a_hl(struct cpu *cpu)
{
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    set_n(cpu->regist, 1);
    uint8_t val = cpu->membus[address];
    cflag_sub_check(cpu->regist->a, val);
    hflag_sub_check(cpu->regist->a, val);
    cpu->regist->a -= val;
    set_z(cpu->regist, cpu->regist->a == 0);
    return 2;
}

//sbc A,r
//x9(8-F)   1 MCycle
int sbc_a_r(struct cpu *cpu, uint8_t *src)
{
    set_n(cpu->regist, 1);
    hflag_sub_set(cpu->regist, cpu->regist->a, *src);
    cflag_sub_set(cpu->regist, cpu->regist->a, *src);
    cpu->regist->a -= *src;
    if (!get_h(cpu->regist))
        hflag_sub_set(cpu->regist, cpu->regist->a, 1);
    if (!get_c(cpu->regist))
        cflag_sub_set(cpu->regist, cpu->regist->a, 1);
    cpu->regist->a--;
    set_z(cpu->regist, cpu->regist->a == 0);
    return 1;
}

//sbc A,(HL)
//x8E   2 MCycle
int sbc_a_hl(struct cpu *cpu)
{
    set_n(cpu->regist, 1);
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    uint8_t val = cpu->membus[address];
    hflag_sub_set(cpu->regist, cpu->regist->a, val);
    cflag_sub_set(cpu->regist, cpu->regist->a, val);
    cpu->regist->a -= val;
    if (!get_h(cpu->regist))
        hflag_sub_set(cpu->regist, cpu->regist->a, 1);
    if (!get_c(cpu->regist))
        cflag_sub_set(cpu->regist, cpu->regist->a, 1);
    cpu->regist->a--;
    set_z(cpu->regist, cpu->regist->a == 0);
    return 2;
}

//and a,r
//xA(0-7)   1 MCycle
int and_a_r(struct cpu *cpu, uint8_t *src)
{
    cpu->regist->a = cpu->regist->a & *src;
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 1);
    set_c(cpu->regist, 0);
    set_z(cpu->regist, cpu->regist->a);
    return 1;
}

//and a,(HL)
//x6A   2 MCycle
int and_a_hl(struct cpu *cpu)
{
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    cpu->regist->a = cpu->regist->a & cpu->membus[address];
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 1);
    set_c(cpu->regist, 0);
    set_z(cpu->regist, cpu->regist->a);
    return 2;
}

//xor A,r
//xA(8-F)   1 MCycle
int xor_a_r(struct cpu *cpu, uint8_t *src)
{
    cpu->regist->a = cpu->regist->a ^ *src;
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 0);
    set_c(cpu->regist, 0);
    set_z(cpu->regist, cpu->regist->a);
    return 1;
}

//xor A,(HL)
//xAE   2 MCycle
int xor_a_hl(struct cpu *cpu)
{
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    cpu->regist->a = cpu->regist->a ^ cpu->membus[address];
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 0);
    set_c(cpu->regist, 0);
    set_z(cpu->regist, cpu->regist->a);
    return 2;
}

//or A,r
//xB(0-7)   1 MCycle
int or_a_r(struct cpu *cpu, uint8_t *src)
{
    cpu->regist->a = cpu->regist->a | *src;
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 0);
    set_c(cpu->regist, 0);
    set_z(cpu->regist, cpu->regist->a);
    return 1;
}

//or A,(HL)
//xB6   2 MCycle
int or_a_hl(struct cpu *cpu)
{
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    cpu->regist->a = cpu->regist->a | cpu->membus[address];
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 0);
    set_c(cpu->regist, 0);
    set_z(cpu->regist, cpu->regist->a);
    return 2;
}

//cp A,r
//xB(8-F)   1 MCycle
int cp_a_r(struct cpu *cpu, uint8_t *src)
{
    set_n(cpu->regist, 1);
    cflag_sub_set(cpu->regist, cpu->regist->a, *src);
    hflag_sub_set(cpu->regist, cpu->regist->a, *src);
    set_z(cpu->regist, cpu->regist->a == 0);
    return 1;
}

//cp A,(HL)
//xBE   2 MCycle
int cp_a_hl(struct cpu *cpu)
{
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    set_n(cpu->regist, 1);
    uint8_t val = cpu->membus[address];
    cflag_sub_check(cpu->regist->a, val);
    hflag_sub_check(cpu->regist->a, val);
    set_z(cpu->regist, cpu->regist->a == 0);
    return 2;
}

//daa A
//x27	1 MCycle
int daa(struct cpu *cpu)
{
	uint8_t *a = &cpu->regist->a;
	if (!get_n(cpu->regist))	//Additioncase
	{
		if (get_c(cpu->regist) || *a > 0x99) //check high nibble
		{
			*a += 0x60;
			set_c(cpu->regist, 1);
		}

		if (get_h(cpu->regist)|| (*a & 0x0F) > 0x09)	//check low nibble
		{
			*a += 0x06;
		}
	}

	else	//Subtraction case
	{
		if (get_c(cpu->regist))
			*a -= 0x60;
		if (get_h(cpu->regist))
			*a -= 0x06;
	}

	return 1;
}



//cpl
//x2F	1 MCycle
int cpl(struct cpu *cpu)
{
	cpu->regist->a = ~cpu->regist->a;
	set_n(cpu->regist, 1);
	set_h(cpu->regist, 1);
	return 1;
}
