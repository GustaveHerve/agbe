#ifndef SYNC_H
#define SYNC_H

#include <stdint.h>
struct cpu;

int64_t get_nanoseconds(void);

void synchronize(struct cpu *cpu);

#endif
