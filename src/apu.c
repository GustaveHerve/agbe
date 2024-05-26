#include <stdlib.h>
#include "apu.h"

void apu_init(struct cpu *cpu, struct apu *apu)
{
    apu->cpu = cpu;
    apu->div_apu = 0;
}

void apu_free(struct apu *apu)
{
    free(apu);
}

void apu_off(struct apu *apu)
{
}
