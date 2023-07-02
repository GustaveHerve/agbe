#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

//rlc
//x0(0-7)   2 MCycle
int rlc(struct cpu *cpu, uint8_t *dest)
{
	rotl(dest);
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 0);
	cflag_rotl_set(cpu->regist, *dest);
	set_z(cpu->regist, *dest == 0);
    return 2;
}

//rlc (HL)
//x06   4 MCycle
int rlc_hl(struct cpu *cpu)
{
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    uint8_t *dest = &cpu->membus[address];
    rotl(dest);
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 0);
	cflag_rotl_set(cpu->regist, *dest);
	set_z(cpu->regist, *dest == 0);
    return 4;
}

//rrc
//x0(8-F)   2 MCycle
int rrc(struct cpu *cpu, uint8_t *dest)
{
    rotr(dest);
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 0);
    cflag_rotr_set(cpu->regist, *dest);
    set_z(cpu->regist, *dest == 0);
    return 2;
}

//rrc (HL)
//x0E   4 MCycle
int rrc_hl(struct cpu *cpu)
{
    uint16_t address = convert_8to16(&cpu->regist->h, &cpu->regist->l);
    uint8_t *dest = &cpu->membus[address];
    rotr(dest);
    set_n(cpu->regist, 0);
    set_h(cpu->regist, 0);
    cflag_rotr_set(cpu->regist, *dest);
    set_z(cpu->regist, *dest == 0);
    return 4;
}
