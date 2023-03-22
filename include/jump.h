#ifndef JUMP_H
#define JUMP_H

int jr_e(struct cpu *gb_cpu);
int jr_nz_e(struct cpu *gb_cpu);
int jr_z_e(struct cpu *gb_cpu);
int jr_nc_e(struct cpu *gb_cpu);
int jr_c_e(struct cpu *gb_cpu);

#endif
