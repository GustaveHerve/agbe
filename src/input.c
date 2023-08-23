#include "cpu.h"

void handle_input_up(struct cpu *cpu, int bit)
{
    if ((cpu->membus[0xFF00] >> 4 & 0x01) == 0x01)
    {
    }
}
