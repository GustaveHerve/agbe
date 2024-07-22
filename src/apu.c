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

static void ch1_tick_m(struct apu *apu)
{

}

void apu_tick_m(struct apu *apu)
{

}
