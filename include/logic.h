#ifndef LOGIC_H
#define LOGIC_H

#include "cpu.h"

int inc_r(struct cpu *cpu, uint8_t *dest);
int inc_hl(struct cpu *cpu);

int inc_rr(struct cpu *cpu, uint8_t *hi, uint8_t *lo);
int inc_sp(struct cpu *cpu);

int dec_r(struct cpu *cpu, uint8_t *dest);
int dec_hl(struct cpu *cpu);

int dec_rr(struct cpu *cpu, uint8_t *hi, uint8_t *lo);
int dec_sp(struct cpu *cpu);

int add_a_r(struct cpu *cpu, uint8_t *src);
int add_a_hl(struct cpu *cpu);
int add_a_n(struct cpu *cpu);

int adc_a_r(struct cpu *cpu, uint8_t *src);
int adc_a_hl(struct cpu *cpu);
int adc_a_n(struct cpu *cpu);

int add_hl_rr(struct cpu *cpu, uint8_t *hi, uint8_t *lo);
int add_hl_sp(struct cpu *cpu);

int add_sp_e8(struct cpu *cpu);

int sub_a_r(struct cpu *cpu, uint8_t *src);
int sub_a_hl(struct cpu *cpu);
int sub_a_n(struct cpu *cpu);

int sbc_a_r(struct cpu *cpu, uint8_t *src);
int sbc_a_hl(struct cpu *cpu);
int sbc_a_n(struct cpu *cpu);

int and_a_r(struct cpu *cpu, uint8_t *src);
int and_a_hl(struct cpu *cpu);
int and_a_n(struct cpu *cpu);

int xor_a_r(struct cpu *cpu, uint8_t *src);
int xor_a_hl(struct cpu *cpu);
int xor_a_n(struct cpu *cpu);

int or_a_r(struct cpu *cpu, uint8_t *src);
int or_a_hl(struct cpu *cpu);
int or_a_n(struct cpu *cpu);

int cp_a_r(struct cpu *cpu, uint8_t *src);
int cp_a_hl(struct cpu *cpu);
int cp_a_n(struct cpu *cpu);

int cpl(struct cpu *cpu);
int daa(struct cpu *cpu);

#endif
