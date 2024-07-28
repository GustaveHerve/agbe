#include <stdlib.h>
#include "apu.h"

static unsigned int duty_table[][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 0, 1, 1 },
    { 0, 0, 0, 0, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 0, 0 },
};

#define FREQUENCY(CH_NUMBER) ((apu->cpu->membus[NR##CH_NUMBER##4] & 0x07) << 8 \
        | apu->cpu->membus[NR##CH_NUMBER##3])

#define DIV_APU_MASK (1 << 5)

#define PERIOD_MASK (0x7 << 0)
#define ENVELOPE_DIR_MASK (0x1 << 3)
#define INITIAL_VOLUME_MASK (0xF << 4)

#define ENVELOPE_DIR_DECREMENT 0
#define ENVELOPE_DIR_INCREMENT 1

#define SWEEP_SHIFT_MASK (0x7 << 0)
#define SWEEP_DIR_MASK (0x1 << 3)
#define SWEEP_PERIOD_MASK (0x7 << 4)

#define SWEEP_DIR_INCREMENT 0
#define SWEEP_DIR_DECREMENT 1

struct ch_env
{
    unsigned int period_timer;
    unsigned int current_volume;
};

void apu_init(struct cpu *cpu, struct apu *apu)
{
    apu->cpu = cpu;
    apu->ch2 = calloc(1, sizeof(struct ch2));
    apu->fs_pos = 0;
}

void apu_free(struct apu *apu)
{
    free(apu->ch2);
    free(apu);
}

static void volume_env_clock(struct apu *apu, struct ch_env *ch, unsigned int nrx2_addr)
{
    uint8_t nrx2 = apu->cpu->membus[nrx2_addr];

    uint8_t env_direction = nrx2 & ENVELOPE_DIR_MASK;
    uint8_t period = nrx2 & nrx2 & PERIOD_MASK;

    if (period == 0)
        return;

    if (ch->period_timer > 0)
        --ch->period_timer;

    if (ch->period_timer != 0)
        return;

    ch->period_timer = period;

    if (env_direction == ENVELOPE_DIR_INCREMENT && ch->current_volume < 0xF)
        ++ch->current_volume;
    else if (env_direction == ENVELOPE_DIR_DECREMENT && ch->current_volume > 0)
        --ch->current_volume;
}

static unsigned int calculate_frequency(struct apu *apu, uint8_t sweep_shift, uint8_t dir)
{
    struct ch1 *ch1 = apu->ch1;
    unsigned int new_frequency = ch1->shadow_frequency >> sweep_shift;

    if (dir == SWEEP_DIR_DECREMENT)
        new_frequency = ch1->shadow_frequency - new_frequency;
    else
        new_frequency = ch1->shadow_frequency + new_frequency;

    if (new_frequency > 2047)
    {
        turn_channel_off(apu, 1);
    }

    return new_frequency;
}

static void frequency_sweep_clock(struct apu *apu)
{
    struct ch1 *ch1 = apu->ch1;

    uint8_t nr10 = apu->cpu->membus[NR10];

    uint8_t period = nr10 & SWEEP_PERIOD_MASK;
    uint8_t dir = nr10 & SWEEP_DIR_MASK;
    uint8_t shift = nr10 & SWEEP_SHIFT_MASK;

    if (ch1->sweep_timer > 0)
        --ch1->sweep_timer;

    if (ch1->sweep_timer != 0)
        return;

    if (period == 0)
        ch1->sweep_timer = 8;
    else
        ch1->sweep_timer = period;

    if (ch1->sweep_enabled && period != 0)
    {
        unsigned int new_frequency = calculate_frequency(apu, shift, dir);
        if (new_frequency < 2048 && shift != 0)
        {
            apu->cpu->membus[NR13] = new_frequency & 0xFF;
            apu->cpu->membus[NR14] = apu->cpu->membus[NR14] & ~(0x07);
            apu->cpu->membus[NR14] = apu->cpu->membus[NR14] | (new_frequency >> 8);
            ch1->shadow_frequency = new_frequency;

            calculate_frequency(apu, shift, dir);
        }
    }
}

static void frame_sequencer_step(struct apu *apu)
{
    /* Length Counter tick */
    if (apu->fs_pos % 2 == 0)
    {
    }

    /* Volume Envelope tick */
    if (apu->fs_pos == 7)
    {
        /* CH3 doesn't have a sweep functionality */
        volume_env_clock(apu, (struct ch_env *)apu->ch1, NR12);
        volume_env_clock(apu, (struct ch_env *)apu->ch2, NR22);
        volume_env_clock(apu, (struct ch_env *)apu->ch4, NR42);
    }

    /* Frequency Sweep tick */
    if (apu->fs_pos == 2 || apu->fs_pos == 6)
    {
        /* Only CH1 has a sweep functionality */
        frequency_sweep_clock(apu);
    }

    apu->fs_pos = (apu->fs_pos + 1) % 8;
}

static void ch2_tick_m(struct apu *apu)
{
    apu->ch2->frequency_timer -= 4;
    if (apu->ch2->frequency_timer == 0)
    {
        apu->ch2->frequency_timer = (2048 - FREQUENCY(2)) * 4;
        apu->ch2->duty_pos = (apu->ch2->duty_pos + 1) % 8;
    }
}

void apu_tick_m(struct apu *apu)
{
    if (!is_apu_on(apu))
        return;

    struct cpu *cpu = apu->cpu;

    uint16_t div = cpu->internal_div + 4;

    /* DIV bit 5 falling edge detection */
    if ((cpu->previous_div & DIV_APU_MASK) && !(div & DIV_APU_MASK))
        frame_sequencer_step(apu);

    ch2_tick_m(apu);
}
