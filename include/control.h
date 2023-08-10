#ifndef CONTROL_H
#define CONTROL_H

struct cpu;

int nop();
int stop(struct cpu *cpu);
int halt(struct cpu *cpu);
int ccf(struct cpu *cpu);
int scf(struct cpu *cpu);
int di(struct cpu *cpu);
int ei(struct cpu *cpu);

#endif
