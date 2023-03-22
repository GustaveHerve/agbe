#ifndef CPU_H
#define CPU_H

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
	struct cpu_register *regist;
	uint8_t *membus; //16-bit address bus that stores ROM RAM I/O
};

void cpu_init(struct cpu *new_cpu);
void cpu_free(struct cpu *todelete);



#endif
