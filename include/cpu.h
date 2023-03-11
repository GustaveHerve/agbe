#ifndef CPU_H
#define CPU_H

struct cpu_register
{
	uint16_t af;
	uint16_t bc;
	uint16_t de;
	uint16_t hl;
	uint16_t sp; //full 16 bit
	uint16_t pc; //full 16 bit
};

struct cpu
{
	struct cpu_register *regist;
	uint8_t *membus; //16-bit address bus that stores ROM RAM I/O
};

void cpu_init(struct cpu *new_cpu);
void cpu_free(struct cpu *todelete);

void set_z(struct cpu_register *regist, int value);
void set_n(struct cpu_register *regist, int value);
void set_h(struct cpu_register *regist, int value);
void set_c(struct cpu_register *regist, int value);

int nop();
int ld_16bit(struct cpu *gb_cpu, uint16_t *dest);
int inc_16bit(uint16_t *dest);
int inc_r(struct cpu *gb_cpu, uint8_t *dest);
uint16_t convert_8to16(uint8_t hi, uint8_t lo);

#endif
