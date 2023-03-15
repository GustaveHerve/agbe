#ifndef UTILS_H
#define UTILS_H

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
