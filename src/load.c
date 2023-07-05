#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"
#include "emulation.h"


//ld (BC),A
//x02	2 MCycle
int ld_bc_a(struct cpu *cpu)
{
	uint16_t address = 0;
    address = convert_8to16(&cpu->regist->b, &cpu->regist->c);
    write_mem(cpu, address, cpu->regist->a);
	return 2;
}

//ld r,r
//		1 MCycle
int ld_r_r(uint8_t *dest, uint8_t *src)
{
	*dest = *src;
	return 1;
}

//ld r,u8
//x(0-3)(6 or E)	2 MCycle
int ld_r_u8(struct cpu *cpu, uint8_t *dest)
{
	cpu->regist->pc++;
	*dest = read_mem(cpu, cpu->regist->pc);
	return 2;
}

//ld (HL),u8
//x36   3 MCycle
int ld_hl_u8(struct cpu *cpu)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
	cpu->regist->pc++;
    uint8_t n = read_mem(cpu, cpu->regist->pc);
	write_mem(cpu, address, n);
	return 3;
}

//ld r,(rr)
//x(0-1)A	2 MCycle
int ld_r_n(struct cpu *cpu, uint8_t *hi, uint8_t *lo)
{
	cpu->regist->pc++;
    uint16_t address = convert_8to16(hi, lo);
    uint8_t n = read_mem(cpu, address);
    cpu->regist->a = n;
	return 2;
}

//ld (HL),r
//x7(0-5)   2 MCycle
int ld_hl_r(struct cpu *cpu, uint8_t *src)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    write_mem(cpu, address, *src);
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

//ld (nn),A
//xEA   4 MCycle
int ld_nn_a(struct cpu *cpu)
{
    cpu->regist->pc++;
    uint8_t lo = read_mem(cpu, cpu->regist->pc);
    cpu->regist->pc++;
    uint8_t hi = read_mem(cpu, cpu->regist->pc);
    uint16_t address = convert_8to16(&hi, &lo);
    write_mem(cpu, address, cpu->regist->a);
    return 4;
}

//ld A,(nn)
//xFA   4 MCycle
int ld_a_nn(struct cpu *cpu)
{
    cpu->regist->pc++;
    uint8_t lo = read_mem(cpu, cpu->regist->pc);
    cpu->regist->pc++;
    uint8_t hi = read_mem(cpu, cpu->regist->pc);
    uint16_t address = convert_8to16(&hi, &lo);
    cpu->regist->a = read_mem(cpu, address);;
    return 4;
}

//ldi (HL+),A
//x22	2 MCycle
int ldi_hl_a(struct cpu *cpu)
{
	uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    write_mem(cpu, address, cpu->regist->a);
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
    write_mem(cpu, address, cpu->regist->a);
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
	cpu->regist->a = read_mem(cpu, address);
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
	cpu->regist->a = read_mem(cpu, address);
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
int ld_rr_nn(struct cpu *cpu, uint8_t *hi, uint8_t *lo)
{
	cpu->regist->pc++;
	*lo = read_mem(cpu, cpu->regist->pc);
	cpu->regist->pc++;
	*hi = read_mem(cpu, cpu->regist->pc);
	return 3;
}

//ld sp,nn
//x31	3 MCycle
int ld_sp_nn(struct cpu *cpu)
{
	cpu->regist->pc++;
	uint8_t lo = read_mem(cpu, cpu->regist->pc);
	cpu->regist->pc++;
	uint8_t hi = read_mem(cpu, cpu->regist->pc);
	cpu->regist->sp = convert_8to16(&hi, &lo);
	return 3;
}

//ld (nn),SP
//x08	5 MCycle
int ld_nn_sp(struct cpu *cpu)
{
	cpu->regist->pc++;
	uint8_t lo = read_mem(cpu, cpu->regist->pc);
	cpu->regist->pc++;
	uint8_t hi = read_mem(cpu, cpu->regist->pc);
	uint16_t address = convert_8to16(&hi, &lo);
    write_mem(cpu, address, regist_lo(&cpu->regist->sp));
    write_mem(cpu, address+1, regist_hi(&cpu->regist->sp));
	return 5;
}

//ld HL,SP+e8
//xF8   3 MCycle
int ld_hl_spe8(struct cpu *cpu)
{
    cpu->regist->pc++;
    uint8_t offset = read_mem(cpu, cpu->regist->pc);
    uint8_t p = regist_lo(&cpu->regist->sp);
    hflag_add_set(cpu->regist, p, offset);
    cflag_add_set(cpu->regist, p, offset);
    set_z(cpu->regist, 0);
    set_n(cpu->regist, 0);
    uint16_t res = cpu->regist->sp + offset;
    //TODO add tick 1 MCycle (nothing)
    cpu->regist->h = regist_hi(&res);
    cpu->regist->l = regist_lo(&res);
    return 3;
}

//ld SP,HL
//xF9   2 MCycle
int ld_sp_hl(struct cpu *cpu)
{
    cpu->regist->sp = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    //TODO add tick 1 MCycle (nothing)
    return 2;
}

int ldh_n_a(struct cpu *cpu)
{
    cpu->regist->pc++;
    uint8_t offset = read_mem(cpu, cpu->regist->pc);
    write_mem(cpu, 0xFF00 + offset, cpu->regist->a);
    return 3;
}

int ldh_a_n(struct cpu *cpu)
{
    cpu->regist->pc++;
    uint8_t offset = read_mem(cpu, cpu->regist->pc);
    cpu->regist->a = read_mem(cpu, 0xFF00 + offset);
    return 3;
}

int ldh_a_c(struct cpu *cpu)
{
    cpu->regist->a = read_mem(cpu, 0xFF00 + cpu->regist->c);
    return 2;
}

int ldh_c_a(struct cpu *cpu)
{
    write_mem(cpu, 0xFF00 + cpu->regist->c, cpu->regist->a);
    return 3;
}

int pop_rr(struct cpu *cpu, uint8_t *hi, uint8_t *lo)
{
    uint8_t lo_a = read_mem(cpu, cpu->regist->sp);
    cpu->regist->sp++;
    uint8_t hi_a = read_mem(cpu, cpu->regist->sp);
    cpu->regist->sp++;
    uint16_t address = convert_8to16(&hi_a, &lo_a);
    *lo = cpu->membus[address];
    *hi = cpu->membus[address + 1];
    return 3;
}

int pop_af(struct cpu *cpu)
{
    uint8_t lo_a = read_mem(cpu, cpu->regist->sp);
    cpu->regist->sp++;
    uint8_t hi_a = read_mem(cpu, cpu->regist->sp);
    cpu->regist->sp++;
    uint16_t address = convert_8to16(&hi_a, &lo_a);
    cpu->regist->f = cpu->membus[address];
    cpu->regist->a = cpu->membus[address + 1];

    set_z(cpu->regist, lo_a >> 7 & 0x1);
    set_n(cpu->regist, lo_a >> 6 & 0x1);
    set_h(cpu->regist, lo_a >> 5 & 0x1);
    set_c(cpu->regist, lo_a >> 4 & 0x1);
    return 3;
}

int push_rr(struct cpu *cpu, uint8_t *hi, uint8_t *lo)
{
    //TODO add tick 1 MCycle (nothing)
    cpu->regist->sp--;
    write_mem(cpu, cpu->regist->sp, *hi);
    cpu->regist->sp--;
    write_mem(cpu, cpu->regist->sp, *lo);
    return 4;
}
