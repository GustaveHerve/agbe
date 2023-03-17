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


/////////////////
//Instruction set
/////////////////

//nop
//x00	1 MCycle
int nop()
{
	return 1;
}

//sto
//x10	1 MCycle
int stop()
{
	//TODO
	return 1;
}

//ccf
//x3F	1 MCycle
int ccf(struct cpu *gb_cpu)
{
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	gb_cpu->regist->f ^= 0x10;
	return 1;
}

//scf
//x37	1 MCycle
int scf(struct cpu *gb_cpu)
{
	set_c(gb_cpu->regist, 1);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	return 1;
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

// add HL,rr
// x(0-2)9	2 MCycle
int add_hl_rr(struct cpu *gb_cpu, uint8_t *hi, uint8_t *lo)
{
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
	set_n(gb_cpu->regist, 0);
	hflag16_add_set(gb_cpu->regist, convert_8to16(&gb_cpu->regist->h,
				&gb_cpu->regist->l), gb_cpu->regist->sp);
	uint16_t sum = gb_cpu->regist->sp +
		convert_8to16(&gb_cpu->regist->h, &gb_cpu->regist->l);
	gb_cpu->regist->h = regist_hi(&sum);
	gb_cpu->regist->l = regist_lo(&sum);
	return 2;
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

/////
//Shift and Rotations
/////

//rlca A
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

//rrca A
//x0F	1 MCycle
int rrca(struct cpu *gb_cpu)
{
	rotr(&gb_cpu->regist->a);
	set_z(gb_cpu->regist, 0);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	cflag_rotr_set(gb_cpu->regist, gb_cpu->regist->a);
	return 1;
}

//rra A
//x1F	1 MCycle
int rra(struct cpu *gb_cpu)
{
	rotr_carry(gb_cpu->regist, &gb_cpu->regist->a);
	set_z(gb_cpu->regist, 0);
	set_n(gb_cpu->regist, 0);
	set_h(gb_cpu->regist, 0);
	return 1;
}

////////////


//cpl
//x2F	1 MCycle
int cpl(struct cpu *gb_cpu)
{
	gb_cpu->regist->a = ~gb_cpu->regist->a;
	set_n(gb_cpu->regist, 1);
	set_h(gb_cpu->regist, 1);
	return 1;
}

//////////////
///Jump instructions
/////////////

//jr e (signed 8 bit)
//x18	3 MCycle
int jr_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	gb_cpu->regist->pc = gb_cpu->regist->pc + e;

	return 3;
}

//jr nz e (signed 8 bit)
//x20	2 MCycle if condition false, 3 MCycle if condition true
int jr_nz_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	if (!get_z(gb_cpu->regist))
	{
		gb_cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

//jr z e (signed 8 bit)
//x28	2 MCycle if condition false, 3 MCycle if condition true
int jr_z_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	if (get_z(gb_cpu->regist))
	{
		gb_cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

//jr nc e (signed 8 bit)
//x30	2 MCycle if condition false, 3 MCycle if condition true
int jr_nc_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	if (!get_c(gb_cpu->regist))
	{
		gb_cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

//jr c e (signed 8 bit)
//x38	2 MCycle if condition false, 3 MCycle if condition true
int jr_c_e(struct cpu *gb_cpu)
{
	gb_cpu->regist->pc++;
	gb_cpu->regist->pc++;
	int8_t e = gb_cpu->membus[gb_cpu->regist->pc];
	if (get_c(gb_cpu->regist))
	{
		gb_cpu->regist->pc += e;
		return 3;
	}

	return 2;
}

