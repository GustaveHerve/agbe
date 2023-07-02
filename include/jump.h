#ifndef JUMP_H
#define JUMP_H

#include <stdlib.h>

int jr_e8(struct cpu *cpu);
int jr_cc_e8(struct cpu *cpu);
/*
int jr_nz_e(struct cpu *cpu);
int jr_z_e(struct cpu *cpu);
int jr_nc_e(struct cpu *cpu);
int jr_c_e(struct cpu *cpu);
*/
int ret(struct cpu *cpu);
int ret_cc(struct cpu *cpu, int cc);
int reti(struct cpu *cpu);

int jp_hl(struct cpu *cpu);
int jp_nn(struct cpu *cpu);
int jp_cc_nn(struct cpu *cpu, int cc);

int call_nn(struct cpu *cpu);
int call_cc_nn(struct cpu *cpu, int cc);

int rst(struct cpu *cpu, uint8_t vec);

#endif
