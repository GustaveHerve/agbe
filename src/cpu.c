#include <stdlib.h>
#include "cpu.h"
#include "ppu.h"
#include "control.h"
#include "jump.h"
#include "load.h"
#include "utils.h"
#include "emulation.h"
#include "mbc.h"

#define MEMBUS_SIZE 65536 // In bytes

#define TAC_TIMER_ENABLED (0x1 << 2)
#define TAC_CLOCK_SELECT 0x3

int get_ie(struct cpu *cpu, int bit);

void cpu_init(struct cpu *cpu, struct renderer *rend, char *rom_path)
{
	cpu->regist = malloc(sizeof(struct cpu_register));
	cpu->membus = calloc(MEMBUS_SIZE, sizeof(uint8_t));
    cpu->mbc = malloc(sizeof(struct mbc));
    cpu->ppu = malloc(sizeof(struct ppu));

    mbc_init(cpu->mbc, rom_path);
    ppu_init(cpu->ppu, cpu, rend);

    cpu->ime = 0;
    cpu->ie = &cpu->membus[0xFFFF];
    cpu->_if = &cpu->membus[0xFF0F];
    cpu->div = &cpu->membus[0xFF04];
    cpu->tima = &cpu->membus[0xFF05];
    cpu->tma = &cpu->membus[0xFF06];
    cpu->tac = &cpu->membus[0xFF07];
    cpu->halt = 0;
    cpu->stop = 0;

    cpu->previous_div = 0;
    cpu->internal_div = 0;
    cpu->serial_clock = 0;
    cpu->serial_acc = 0;

    //Joypad
    cpu->joyp_a = 0xFF;
    cpu->joyp_d = 0xFF;

    cpu->regist->a = 0x00;
    cpu->regist->f = 0x00;
    cpu->regist->b = 0x00;
    cpu->regist->c = 0x00;
    cpu->regist->h = 0x00;
    cpu->regist->l = 0x00;
    cpu->regist->pc = 0x00;
    cpu->regist->sp = 0x00;

    *cpu->ie = 0x00;
    *cpu->_if = 0xE1;
    *cpu->div = 0x00;
    *cpu->tima = 0x00;
    *cpu->tma = 0x00;
    *cpu->tac = 0x00;

    cpu->disabling_timer = 0;
    cpu->schedule_tima_overflow = 0;

    cpu->boot = &cpu->membus[0xFF50];

    cpu->sb = &cpu->membus[0xFF01];
    cpu->sc = &cpu->membus[0xFF02];
}

void cpu_init_registers(struct cpu *cpu, int checksum)
{
    cpu->regist->a = 0x01;
    set_z(cpu->regist, 1);
    set_n(cpu->regist, 0);
    if (checksum == 0x00)
    {
        set_h(cpu->regist, 0);
        set_c(cpu->regist, 0);
    }
    else
    {
        set_h(cpu->regist, 1);
        set_c(cpu->regist, 1);
    }
    cpu->regist->b = 0x00;
    cpu->regist->c = 0x13;
    cpu->regist->d = 0x00;
    cpu->regist->e = 0xD8;
    cpu->regist->h = 0x01;
    cpu->regist->l = 0x4D;
    cpu->regist->pc = 0x0100;
    cpu->regist->sp = 0xFFFE;
}

void cpu_free(struct cpu *todelete)
{
    ppu_free(todelete->ppu);
	free(todelete->membus);
    free(todelete->regist);
    mbc_free(todelete->mbc);
	free(todelete);
}

static unsigned int clock_masks[] = { 1 << 9, 1 << 3, 1 << 5, 1 << 7 };

void update_timers(struct cpu *cpu)
{
    if (cpu->schedule_tima_overflow)
    {
        set_if(cpu, 2);
        *cpu->tima = *cpu->tma;
        cpu->schedule_tima_overflow = 0;
    }

    if (!cpu->stop)
    {
        cpu->internal_div += 4;
        *cpu->div = cpu->internal_div >> 8;
    }

    uint8_t previous_tima = *cpu->tima;
    unsigned int selected_clock = *cpu->tac & TAC_CLOCK_SELECT;
    unsigned int clock_mask = clock_masks[selected_clock];
    if (*cpu->tac & TAC_TIMER_ENABLED || cpu->disabling_timer)
    {
        /* Increase TIMA on falling edge */
        if (cpu->disabling_timer)
        {
            /* Handle TIMA increment quirk when disabling timer in TAC */
            if (cpu->previous_div & clock_mask)
                ++(*cpu->tima);
            cpu->disabling_timer = 0;
        }
        else if ((cpu->previous_div & clock_mask) && !(cpu->internal_div & clock_mask))
            ++(*cpu->tima);

        /* TIMA Overflow */
        if (previous_tima > *cpu->tima)
            cpu->schedule_tima_overflow = 1; /* Schedule an interrupt for next Mcycle*/
    }

    cpu->previous_div = cpu->internal_div;
}

int check_interrupt(struct cpu *cpu)
{
    if (!cpu->halt && !cpu->ime)
        return 0;

    //Joypad check
    if (((cpu->membus[0xFF00] >> 5 & 0x01) == 0x00) ||
        (cpu->membus[0xFF00] >> 4 & 0x01) == 0x00)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (((cpu->membus[0xFF00] >> 5 & 0x01) == 0x00) ||
                (cpu->membus[0xFF00] >> 4 & 0x01) == 0x00)
            {
                if (((cpu->membus[0xFF00] >> i) & 0x01) == 0x00)
                    set_if(cpu, 4);
            }
        }
    }

    for (int i = 0; i < 5; ++i)
    {
        if (get_if(cpu, i) && get_ie(cpu, i))
        {
            cpu->halt = 0;
            if (!cpu->ime) // Wake up from halt with IME = 0
                return 0;
            handle_interrupt(cpu, i);
        }
    }
    return 1;
}

                                /* VBlank, LCD STAT, Timer, Serial, Joypad */
static unsigned int handler_vectors[] = { 0x40, 0x48, 0x50, 0x58, 0x60 };

int handle_interrupt(struct cpu *cpu, int bit)
{
    clear_if(cpu, bit);
    cpu->ime = 0;
    tick_m(cpu);
    tick_m(cpu);
    uint8_t lo = regist_lo(&cpu->regist->pc);
    uint8_t hi = regist_hi(&cpu->regist->pc);
    --cpu->regist->sp;
    write_mem(cpu, cpu->regist->sp, hi);
    --cpu->regist->sp;
    write_mem(cpu, cpu->regist->sp, lo);
    uint16_t handler = handler_vectors[bit];
    cpu->regist->pc = handler;
    tick_m(cpu);
    return 1;
}

//Interrupt utils

int get_if(struct cpu *cpu, int bit)
{
    return (*cpu->_if >> bit) & 0x01;
}

void set_if(struct cpu *cpu, int bit)
{
    *cpu->_if = *cpu->_if | (0x01 << bit);
}

void clear_if(struct cpu *cpu, int bit)
{
    *cpu->_if = *cpu->_if & ~(0x01 << bit);
}

int get_ie(struct cpu *cpu, int bit)
{
    return (*cpu->ie >> bit) & 0x01;
}

void set_ie(struct cpu *cpu, int bit)
{
    *cpu->ie = *cpu->ie | (0x01 << bit);
}

void clear_ie(struct cpu *cpu, int bit)
{
    *cpu->ie = *cpu->ie & ~(0x01 << bit);
}
