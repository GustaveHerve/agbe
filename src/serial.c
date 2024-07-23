#include "cpu.h"
#include <stddef.h>

void serial_transfer(struct cpu *cpu)
{
    *cpu->sb = 0x81;
}

void transfer_complete(struct cpu *cpu)
{
    *cpu->sb &= ~0x80;
}
