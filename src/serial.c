#include "cpu.h"
#include <stddef.h>

void serial_transfer(struct cpu *cpu)
{
    *cpu->sb = (*cpu->sb << 1) | 0x01;
}
