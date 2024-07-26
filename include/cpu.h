#ifndef CPU_H
#define CPU_H

#include <stdint.h>

struct ppu;
struct renderer;
struct mbc;

struct cpu_register
{
	uint8_t a;
	uint8_t f;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;

	uint16_t sp;
	uint16_t pc;
};

struct cpu
{
    int running;
    struct ppu *ppu;
	struct cpu_register *regist;
	uint8_t *membus;

    int ime;
    uint8_t *ie;
    uint8_t *_if;

    uint16_t previous_div;
    uint16_t internal_div;
    uint8_t *div;
    uint8_t *tima;
    uint8_t *tma;
    uint8_t *tac;

    uint8_t disabling_timer;
    uint8_t schedule_tima_overflow;

    int halt;
    int stop;

    uint8_t serial_clock;
    uint8_t serial_acc;

    struct mbc *mbc;

    uint8_t joyp_a;
    uint8_t joyp_d;

    uint8_t *boot;

    uint8_t *sb;
    uint8_t *sc;
};

void cpu_init(struct cpu *new_cpu, struct renderer *rend, char *rom_path);
void cpu_start(struct cpu *cpu);
void cpu_free(struct cpu *todelete);

void cpu_init_registers(struct cpu *cpu, int checksum);

void update_timers(struct cpu *cpu);

#endif
