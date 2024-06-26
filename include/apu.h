#ifndef APU_H
#define APU_H

#include <stdint.h>
#include "cpu.h"

// Global control registers
#define NR50 0xFF24
#define NR51 0xFF25
#define NR52 0xFF26

// Sound Channel 1 - Pulse with period sweep
#define NR10 0xFF10
#define NR11 0xFF11
#define NR12 0xFF12
#define NR13 0xFF13
#define NR14 0xFF14

// Sound Channel 2 - Pulse
#define NR21 0xFF16
#define NR22 0xFF17
#define NR23 0xFF18
#define NR24 0xFF19

// Sound Channel 3 - Wave output
#define NR30 0xFF1A
#define NR31 0xFF1B
#define NR32 0xFF1C
#define NR33 0xFF1D
#define NR34 0xFF1E

// Sound Channel 4 - Noise
#define NR41 0xFF20
#define NR42 0xFF21
#define NR43 0xFF22
#define NR44 0xFF23

struct apu
{
    struct cpu *cpu;
    int div_apu;
};

void apu_init(struct cpu *cpu, struct apu *apu);

void apu_free(struct apu *apu);

void apu_tick_m(struct apu *apu);

static inline uint8_t is_apu_off(struct apu *apu)
{
    return apu->cpu->membus[NR52] >> 7;
}

static inline uint8_t is_channel_on(struct apu *apu, uint8_t number)
{
    return (apu->cpu->membus[NR52] >> (number - 1)) & 0x01;
}

#endif
