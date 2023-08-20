#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"

int get_z(struct cpu_register *regist);
void set_z(struct cpu_register *regist, int value);

int get_n(struct cpu_register *regist);
void set_n(struct cpu_register *regist, int value);

int get_h(struct cpu_register *regist);
void set_h(struct cpu_register *regist, int value);

int get_c(struct cpu_register *regist);
void set_c(struct cpu_register *regist, int value);


int hflag_add_check(uint8_t a, uint8_t b);
void hflag_add_set(struct cpu_register *regist, uint8_t a, uint8_t b);
int hflag_sub_check(uint8_t a, uint8_t b);
void hflag_sub_set(struct cpu_register *regist, uint8_t a, uint8_t b);

int hflag16_add_set(struct cpu_register *regist, uint16_t a, uint16_t b);


void cflag_rotl_set(struct cpu_register *regist, uint8_t src);
int cflag_rotr_check(uint8_t src);
void cflag_rotr_set(struct cpu_register *regist, uint8_t src);

int cflag_add_check(uint8_t a, uint8_t b);
void cflag_add_set(struct cpu_register *regist, uint8_t a, uint8_t b);
int cflag_sub_check(uint8_t a, uint8_t b);
void cflag_sub_set(struct cpu_register *regist, uint8_t a, uint8_t b);

int cflag16_add_check(uint16_t a, uint16_t b);
void cflag16_add_set(struct cpu_register *regist, uint16_t a, uint16_t b);

uint8_t regist_hi(uint16_t *rr);
uint8_t regist_lo(uint16_t *rr);

uint8_t get_lsb_nibble(uint8_t b);
uint8_t get_msb_nibble(uint8_t b);

uint16_t convert_8to16(uint8_t *hi, uint8_t *lo);


void rotl(uint8_t *src);
void rotl_carry(struct cpu_register *regist, uint8_t *src);

void rotr(uint8_t *src);
void rotr_carry(struct cpu_register *regist, uint8_t *src);
void rotl_16(uint16_t *src);
void rotr_16(uint16_t *src);

#endif
