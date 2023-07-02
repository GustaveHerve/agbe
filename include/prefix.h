#ifndef PREFIX_H
#define PREFIX_H

#include <stdlib.h>

int rlc(struct cpu *cpu, uint8_t *dest);
int rlc_hl(struct cpu *cpu);

int rrc(struct cpu *cpu, uint8_t *dest);
int rrc_hl(struct cpu *cpu);

#endif
