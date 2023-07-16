#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "ppu.h"
#include "utils.h"

void tick(struct cpu *cpu)
{
    //TODO advance one t-state
}

void tick_m(struct cpu *cpu)
{
    //TODO advance one MCycle (4 t-states)
}

uint8_t read_mem(struct cpu *cpu, uint16_t address)
{
    //TODO Verify that address is valid
    //TODO PPU tick +1 MCycle (+4 T-State)
    tick_m(cpu);
    return cpu->membus[address];
}

void write_mem(struct cpu *cpu, uint16_t address, uint8_t val)
{
    //TODO Verify that address is valid
    //TODO PPU tick +1 MCycle (+4 T-State)
    tick_m(cpu);
    cpu->membus[address] = val;
}
