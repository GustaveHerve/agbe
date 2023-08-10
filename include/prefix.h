#ifndef PREFIX_H
#define PREFIX_H

#include <stdlib.h>
struct cpu;

int rlc(struct cpu *cpu, uint8_t *dest);
int rlc_hl(struct cpu *cpu);
int rrc(struct cpu *cpu, uint8_t *dest);
int rrc_hl(struct cpu *cpu);

int rl(struct cpu *cpu, uint8_t *dest);
int rl_hl(struct cpu *cpu);
int rr(struct cpu *cpu, uint8_t *dest);
int rr_hl(struct cpu *cpu);

int sla(struct cpu *cpu, uint8_t *dest);
int sla_hl(struct cpu *cpu);
int sra(struct cpu *cpu, uint8_t *dest);
int sra_hl(struct cpu *cpu);

int swap(struct cpu *cpu, uint8_t *dest);
int swap_hl(struct cpu *cpu);

int srl(struct cpu *cpu, uint8_t *dest);
int srl_hl(struct cpu *cpu);

int bit(struct cpu *cpu, uint8_t *dest, int n);
int bit_hl(struct cpu *cpu, int n);

int res(uint8_t *dest, int n);
int res_hl(struct cpu *cpu, int n);

int set(uint8_t *dest, int n);
int set_hl(struct cpu *cpu, int n);

#endif
