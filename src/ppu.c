#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"
#include "ppu.h"

void ppu_init(struct ppu *ppu, struct cpu *cpu)
{
    ppu->cpu = cpu;
    ppu->mode = 2;
}

void ppu_tick(struct ppu *ppu, struct cpu *cpu)
{
    return;
}
