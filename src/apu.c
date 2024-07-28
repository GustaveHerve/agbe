#include <stdlib.h>
#include "apu.h"
#include "utils.h"

static unsigned int duty_table[][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 0, 1, 1 },
    { 0, 0, 0, 0, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 0, 0 },
};

static unsigned int ch4_divisors[] = { 8, 16, 32, 48, 64, 80, 96, 112 };

#define FREQUENCY(CH_NUMBER) ((apu->cpu->membus[NR##CH_NUMBER##4] & 0x07) << 8 \
        | apu->cpu->membus[NR##CH_NUMBER##3])

#define DIV_APU_MASK (1 << 5)

#define ENVELOPE_DIR_DECREMENT 0
#define ENVELOPE_DIR_INCREMENT 1

#define ENV_SWEEP_PERIOD(NRX2) ((NRX2) & 0x7)
#define ENV_DIR(NRX2) (((NRX2) >> 3) & 0x1)
#define ENV_INIT_VOLUME(NRX2) (((NRX2) >> 4) & 0xF)

#define SWEEP_SHIFT(NR10) ((NR10) & 0x7)
#define SWEEP_DIR(NR10) (((NR10) >> 3) & 0x1)
#define SWEEP_PERIOD(NR10) (((NR10) >> 4) & 0x7)

#define SWEEP_DIR_INCREMENT 0
#define SWEEP_DIR_DECREMENT 1

#define WAVE_RAM 0xFF30

#define NOISE_CLOCK_DIVIDER_CODE(NR43) ((NR43) & 0x7)
#define NOISE_LFSR_WIDTH(NR43) (((NR43) >> 3) & 0x1)
#define NOISE_CLOCK_SHIFT(NR43) (((NR43) >> 4) & 0xF)

struct ch_generic
{
    unsigned int length_timer;
    unsigned int period_timer;
    unsigned int current_volume;
    unsigned int env_dir;
    unsigned int env_period;
};

void apu_init(struct cpu *cpu, struct apu *apu)
{
    apu->cpu = cpu;
    apu->ch1 = calloc(1, sizeof(struct ch1));
    apu->ch2 = calloc(1, sizeof(struct ch2));
    apu->ch3 = calloc(1, sizeof(struct ch4));
    apu->ch4 = calloc(1, sizeof(struct ch4));
    apu->fs_pos = 0;
}

void apu_free(struct apu *apu)
{
    free(apu->ch1);
    free(apu->ch2);
    free(apu->ch3);
    free(apu->ch4);
    free(apu);
}

static void length_trigger(struct apu *apu, struct ch_generic *ch)
{
    if ((void *)ch == (void *)&apu->ch3 && ch->length_timer == 0)
        ch->length_timer = 256;
    else if (ch->length_timer == 0)
        ch->length_timer = 64;
}

static unsigned int calculate_frequency(struct apu *apu, uint8_t sweep_shift, uint8_t dir);
static void frequency_sweep_trigger(struct apu *apu, struct ch1 *ch1)
{
    unsigned int nr10 = apu->cpu->membus[NR10];
    unsigned int sweep_shift = SWEEP_SHIFT(nr10);
    unsigned int sweep_period = SWEEP_PERIOD(nr10);

    ch1->shadow_frequency = FREQUENCY(1);
    ch1->sweep_timer = sweep_period;

    if (ch1->sweep_timer == 0)
        ch1->sweep_timer = 8;

    if (sweep_period != 0 || sweep_shift != 0)
        ch1->sweep_enabled = 1;
    else
        ch1->sweep_enabled = 0;

    if (sweep_shift)
        calculate_frequency(apu, sweep_shift, SWEEP_DIR(nr10));
}

static void envelope_trigger(struct apu *apu, struct ch_generic *ch, uint8_t ch_number)
{
    static unsigned int nrx2_addr[] = { NR12, NR22, NR32, NR42 };
    unsigned int nrx2 = nrx2_addr[ch_number - 1];
    ch->current_volume = ENV_INIT_VOLUME(nrx2);
    ch->env_dir = ENV_DIR(nrx2);
    ch->env_period = ENV_SWEEP_PERIOD(nrx2);
    ch->period_timer = ch->env_period;
}

void handle_trigger_event_ch1(struct apu *apu)
{
    length_trigger(apu, (void *)apu->ch1);
    apu->ch1->frequency_timer = (2048 - FREQUENCY(1)) * 4;
    envelope_trigger(apu, (void *)apu->ch1, 1);
    frequency_sweep_trigger(apu, apu->ch1);

    if (is_dac_on(apu, 1))
        turn_channel_on(apu, 1);
    else
        turn_channel_off(apu, 1);
}

void handle_trigger_event_ch2(struct apu *apu)
{
    length_trigger(apu, (void *)apu->ch2);
    apu->ch2->frequency_timer = (2048 - FREQUENCY(2)) * 4;
    envelope_trigger(apu, (void *)apu->ch2, 2);

    if (is_dac_on(apu, 2))
        turn_channel_on(apu, 2);
    else
        turn_channel_off(apu, 2);
}

void handle_trigger_event_ch3(struct apu *apu)
{
    length_trigger(apu, (void *)apu->ch3);
    apu->ch3->frequency_timer = (2048 - FREQUENCY(2)) * 2;
    apu->ch3->wave_pos = 0;

    if (is_dac_on(apu, 3))
        turn_channel_on(apu, 3);
    else
        turn_channel_off(apu, 3);
}

void handle_trigger_event_ch4(struct apu *apu)
{
    length_trigger(apu, (void *)apu->ch4);
    unsigned int nr43 = apu->cpu->membus[NR43];
    unsigned int shift = NOISE_CLOCK_SHIFT(nr43);
    unsigned int divisor_code = NOISE_CLOCK_DIVIDER_CODE(nr43);
    apu->ch4->frequency_timer = ch4_divisors[divisor_code] << shift;
    envelope_trigger(apu, (void *)apu->ch4, 4);

    apu->ch4->lfsr = 0x7FFF;
    if (is_dac_on(apu, 4))
        turn_channel_on(apu, 4);
    else
        turn_channel_off(apu, 4);
}

void enable_timer(struct apu *apu, uint8_t ch_number)
{
    struct ch_generic *ch = NULL;
    unsigned int val = 64;
    unsigned int init_len_mask = 0x3F;
    switch (ch_number)
    {
        case 1:
            ch = (void *) &apu->ch1;
            break;
        case 2:
            ch = (void *) &apu->ch2;
            break;
        case 3:
            ch = (void *) &apu->ch3;
            val = 256;
            init_len_mask = 0xFF;
            break;
        case 4:
            ch = (void *) &apu->ch4;
            break;
    }
    unsigned int nrx1 = NR11 + ((NR21 - NR11) * (ch_number - 1));
    unsigned int initial_length = apu->cpu->membus[nrx1] & init_len_mask;

    ch->length_timer = val - initial_length;
}

static void length_clock(struct apu *apu, struct ch_generic *ch, uint8_t number)
{
    ch->length_timer -= 1;
    if (ch->length_timer == 0)
        turn_channel_off(apu, number);
}

static void volume_env_clock(struct apu *apu, struct ch_generic *ch)
{
    if (ch->env_period == 0)
        return;

    if (ch->period_timer > 0)
        --ch->period_timer;

    if (ch->period_timer != 0)
        return;

    ch->period_timer = ch->env_period;

    if (ch->env_dir == ENVELOPE_DIR_INCREMENT && ch->current_volume < 0xF)
        ++ch->current_volume;
    else if (ch->env_dir == ENVELOPE_DIR_DECREMENT && ch->current_volume > 0)
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

    uint8_t period = SWEEP_PERIOD(nr10);
    uint8_t dir = SWEEP_DIR(nr10);
    uint8_t shift = SWEEP_SHIFT(nr10);

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
        length_clock(apu, (struct ch_generic *)apu->ch1, 1);
        length_clock(apu, (struct ch_generic *)apu->ch2, 2);
        length_clock(apu, (struct ch_generic *)apu->ch3, 3);
        length_clock(apu, (struct ch_generic *)apu->ch4, 4);
    }

    /* Volume Envelope tick */
    if (apu->fs_pos == 7)
    {
        /* CH3 doesn't have an envelope functionality */
        volume_env_clock(apu, (struct ch_generic *)apu->ch1);
        volume_env_clock(apu, (struct ch_generic *)apu->ch2);
        volume_env_clock(apu, (struct ch_generic *)apu->ch4);
    }

    /* Frequency Sweep tick */
    if (apu->fs_pos == 2 || apu->fs_pos == 6)
    {
        /* Only CH1 has a sweep functionality */
        frequency_sweep_clock(apu);
    }

    apu->fs_pos = (apu->fs_pos + 1) % 8;
}

static void ch1_tick_m(struct apu *apu)
{
    for (size_t i = 0; i < 4; ++i)
    {
        apu->ch1->frequency_timer -= 1;
        if (apu->ch1->frequency_timer == 0)
        {
            apu->ch1->frequency_timer = (2048 - FREQUENCY(1)) * 4;
            apu->ch1->duty_pos = (apu->ch2->duty_pos + 1) % 8;
        }
    }
}

static void ch2_tick_m(struct apu *apu)
{
    for (size_t i = 0; i < 4; ++i)
    {
        apu->ch2->frequency_timer -= 1;
        if (apu->ch2->frequency_timer == 0)
        {
            apu->ch2->frequency_timer = (2048 - FREQUENCY(2)) * 4;
            apu->ch2->duty_pos = (apu->ch2->duty_pos + 1) % 8;
        }
    }
}

static void ch3_tick_m(struct apu *apu)
{
    for (size_t i = 0; i < 4; ++i)
    {
        apu->ch3->frequency_timer -= 1;
        if (apu->ch3->frequency_timer == 0)
        {
            apu->ch3->frequency_timer = (2048 - FREQUENCY(3)) * 2;
            apu->ch3->wave_pos = (apu->ch3->wave_pos + 1) % 32;

            unsigned int sample = apu->cpu->membus[WAVE_RAM + (apu->ch3->wave_pos / 2)];
            /* Each byte has two 4-bit samples */
            if (apu->ch3->wave_pos % 2 == 0)
                sample >>= 4;
            else
                sample &= 0x0F;

            apu->ch3->sample_buffer = sample;
        }
    }
}


static void ch4_tick_m(struct apu *apu)
{
    for (size_t i = 0; i < 4; ++i)
    {
        unsigned int nr43 = apu->cpu->membus[NR43];
        unsigned int shift = NOISE_CLOCK_SHIFT(nr43);
        unsigned int divisor_code = NOISE_CLOCK_DIVIDER_CODE(nr43);

        apu->ch4->frequency_timer -= 1;
        if (apu->ch4->frequency_timer == 0)
        {
            apu->ch4->frequency_timer = ch4_divisors[divisor_code] << shift;

            unsigned xor_res = ((apu->ch4->lfsr >> 1) & 0x01) ^ (apu->ch4->lfsr & 0x01);
            apu->ch4->lfsr = (apu->ch4->lfsr >> 1) | (xor_res << 14);

            if (NOISE_LFSR_WIDTH(NR43))
                apu->ch4->lfsr = (apu->ch4->lfsr & ~(1 << 6)) | (xor_res << 6);
        }
    }
}

void apu_tick_m(struct apu *apu)
{
    if (!is_apu_on(apu))
        return;

    struct cpu *cpu = apu->cpu;

    uint16_t div = cpu->internal_div + 4;

    ch1_tick_m(apu);
    ch2_tick_m(apu);
    ch3_tick_m(apu);
    ch4_tick_m(apu);

    /* DIV bit 5 falling edge detection */
    if ((cpu->previous_div & DIV_APU_MASK) && !(div & DIV_APU_MASK))
        frame_sequencer_step(apu);
}
