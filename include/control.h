#ifndef CONTROL_H
#define CONTROL_H

#include "cpu.h"

int nop();
int stop();
int halt();
int ccf(struct cpu *cpu);
int scf(struct cpu *cpu);

#endif
