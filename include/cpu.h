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

	uint16_t sp; //full 16 bit
	uint16_t pc; //full 16 bit
};

struct cpu
{
    int running;
    struct ppu *ppu;
	struct cpu_register *regist;
	uint8_t *membus; //16-bit address bus that stores ROM RAM I/O

    int ime;
    uint8_t *ie;
    uint8_t *_if;

    uint8_t *div;
    uint8_t *tima;
    uint8_t *tma;
    uint8_t *tac;

    int halt;
    int stop;

    int div_timer;
    int tima_timer;

    struct mbc *mbc;

    uint8_t joyp_a;
    uint8_t joyp_d;
};

void cpu_init(struct cpu *new_cpu, struct renderer *rend);
void cpu_start(struct cpu *cpu);
void cpu_free(struct cpu *todelete);

//Interrupts
int check_interrupt(struct cpu *cpu);
int handle_interrupt(struct cpu *cpu, int bit);

int get_if(struct cpu *cpu, int bit);
void set_if(struct cpu *cpu, int bit);
void clear_if(struct cpu *cpu, int bit);

#endif
