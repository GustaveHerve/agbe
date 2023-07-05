#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"

uint8_t read_mem(struct cpu *cpu, uint16_t address)
{
    //TODO Verify that address is valid
    //TODO PPU tick +1 MCycle (+4 T-State)
    return cpu->membus[address];
}

void write_mem(struct cpu *cpu, uint16_t address, uint8_t val)
{
    //TODO Verify that address is valid
    //TODO PPU tick +1 MCycle (+4 T-State)
    cpu->membus[address] = val;
}
