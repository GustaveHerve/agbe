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
	//uint16_t af;
	//uint16_t bc;
	//uint16_t de;
	//uint16_t hl;

	uint16_t sp; //full 16 bit
	uint16_t pc; //full 16 bit
	/*
	uint8_t *sp_lo;
	uint8_t *sp_hi;

	uint8_t *pc_lo;
	uint8_t *pc_hi;
	*/
};

struct cpu
{
	struct cpu_register *regist;
	uint8_t *membus; //16-bit address bus that stores ROM RAM I/O
};

void cpu_init(struct cpu *new_cpu);
void cpu_free(struct cpu *todelete);

int get_z(struct cpu_register *regist);
void set_z(struct cpu_register *regist, int value);

int get_n(struct cpu_register *regist);
void set_n(struct cpu_register *regist, int value);

int get_h(struct cpu_register *regist);
void set_h(struct cpu_register *regist, int value);

int get_c(struct cpu_register *regist);
void set_c(struct cpu_register *regist, int value);


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
