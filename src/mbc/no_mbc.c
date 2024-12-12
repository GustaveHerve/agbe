#include "mbc/no_mbc.h"

#include <stdint.h>

#include "cpu.h"
#include "mbc_base.h"

static void mbc_init(struct mbc_base *mbc_base)
{
    (void)mbc_base;
    /* Nothing to do, all is handled in mbc_base */
    return;
}

static void mbc_free(struct mbc_base *mbc)
{
    (void)mbc;
    /* Nothing to do, all is handled in mbc_base */
    return;
}

static uint8_t read_mbc_rom(struct cpu *cpu, uint16_t address)
{
    struct no_mbc *mbc = (struct no_mbc *)cpu->mbc;
    return mbc->base.rom[address];
}

static uint8_t read_mbc_ram(struct cpu *cpu, uint16_t address)
{
    (void)cpu;
    (void)address;
    /* Invalid read as there is no RAM chip mapped, return 0xFF */
    return 0xFF;
}

static void write_mbc_ram(struct cpu *cpu, uint16_t address, uint8_t val)
{
    (void)cpu;
    (void)address;
    (void)val;
    /* Invalid write as there is no RAM chip mapped, don't do anything */
    return;
}

static void write_mbc(struct cpu *cpu, uint16_t address, uint8_t val)
{
    (void)cpu;
    (void)address;
    (void)val;
    /* There is no register to write to without an MBC, don't do anything */
    return;
}
