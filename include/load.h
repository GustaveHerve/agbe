#ifndef LOAD_H
#define LOAD_H

#include "cpu.h"

int ld_bc_a(struct cpu *cpu);
int ld_r_r(uint8_t *dest, uint8_t *src);
int ld_r_u8(struct cpu *cpu, uint8_t *dest);
int ld_hl_u8(struct cpu *cpu);
int ld_r_n(struct cpu *cpu, uint8_t *hi, uint8_t *lo);
int ld_hl_r(struct cpu *cpu, uint8_t *src);
int ld_r_hl(struct cpu *cpu, uint8_t *dest);
int ld_nn_a(struct cpu *cpu);
int ld_a_nn(struct cpu *cpu);

int ldh_n_a(struct cpu *cpu);
int ldh_a_n(struct cpu *cpu);

int ldh_a_c(struct cpu *cpu);
int ldh_c_a(struct cpu *cpu);

int ldi_hl_a(struct cpu *cpu);
int ldd_hl_a(struct cpu *cpu);

int ldi_a_hl(struct cpu *cpu);
int ldd_a_hl(struct cpu *cpu);

int ld_rr_nn(struct cpu *cpu, uint8_t *hi, uint8_t *lo);
int ld_sp_nn(struct cpu *cpu);
int ld_nn_sp(struct cpu *cpu);

int ld_hl_spe8(struct cpu *cpu);

int pop_rr(struct cpu *cpu, uint8_t *hi, uint8_t *lo);
int pop_af(struct cpu *cpu);

int push_rr(struct cpu *cpu, uint8_t *hi, uint8_t *lo);

#endif
