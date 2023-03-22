#ifndef LOAD_H
#define LOAD_H

#include "cpu.h"

int ld_bc_a(struct cpu *gb_cpu);
int ld_r_r(uint8_t *dest, uint8_t *src);
int ld_r_u8(struct cpu *gb_cpu, uint8_t *dest);
int ld_hl_u8(struct cpu *gb_cpu);
int ld_u8_r(struct cpu *gb_cpu, uint8_t *src);
int ld_hl_r(struct cpu *gb_cpu, uint8_t *src);
int ld_r_hl(struct cpu *gb_cpu, uint8_t *dest);

int ldi_hl_a(struct cpu *gb_cpu);
int ldd_hl_a(struct cpu *gb_cpu);


int ldi_a_hl(struct cpu *gb_cpu);
int ldd_a_hl(struct cpu *gb_cpu);


int ld_rr_u16(struct cpu *gb_cpu, uint8_t *hi, uint8_t *lo);
int ld_sp_u16(struct cpu *gb_cpu);
int ld_nn_sp(struct cpu *gb_cpu);

#endif
