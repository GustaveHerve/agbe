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


int nop();

int ld_rr_u16(struct cpu *gb_cpu, uint8_t *hi, uint8_t *lo);
int ld_bc_a(struct cpu *gb_cpu);
int ld_r_u8(struct cpu *gb_cpu, uint8_t *dest);
int ld_hl_u8(struct cpu *gb_cpu);

int ldi_hl_a(struct cpu *gb_cpu);
int ldd_hl_a(struct cpu *gb_cpu);
int ldi_a_hl(struct cpu *gb_cpu);
int ldd_a_hl(struct cpu *gb_cpu);

int inc_rr(uint8_t *hi, uint8_t *lo);
int inc_rr_sp(uint16_t *dest);
int inc_r(struct cpu *gb_cpu, uint8_t *dest);
int inc_hl(struct cpu *gb_cpu);

int dec_r(struct cpu *gb_cpu, uint8_t *dest);
int dec_hl(struct cpu *gb_cpu);

int rlca(struct cpu *gb_cpu);
int rla(struct cpu *gb_cpu);

#endif
