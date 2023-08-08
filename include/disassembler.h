#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "cpu.h"

int next_op(struct cpu *cpu);
int prefix_op(struct cpu *cpu);

#endif
