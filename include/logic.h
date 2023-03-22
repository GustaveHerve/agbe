#ifndef LOGIC_H
#define LOGIC_H

#include "cpu.h"

int inc_r(struct cpu *gb_cpu, uint8_t *dest);
int inc_hl(struct cpu *gb_cpu);

int inc_rr(uint8_t *hi, uint8_t *lo)
int inc_sp(uint16_t *dest);

int dec_r(struct cpu *gb_cpu, uint8_t *dest);
int dec_hl(struct cpu *gb_cpu);

int dec_rr(uint8_t *hi, uint8_t *lo);
int dec_sp(uint16_t *sp);

//int add_a_r(struct cpu *gb_cpu, uint8_t *src)
//int add_a_hl(struct cpu *gb_cpu);

//int adc_a_r(struct cpu *gb_cpu, uint8_t *src);
//int adc_a_hl(struct cpu *gb_cpu);

int add_hl_rr(struct cpu *gb_cpu, uint8_t *hi, uint8_t *lo);
int add_hl_sp(struct cpu *gb_cpu);


int cpl(struct cpu *gb_cpu);
int daa(struct cpu *gb_cpu);

#endif
