#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "save.h"

static void mbc_init(struct mbc *mbc, char *rom_path)
{
}

static void mbc_free(struct mbc *mbc)
{
}

static void set_mbc(struct cpu *cpu, uint8_t *rom)
{
}

static uint8_t read_mbc_rom(struct cpu *cpu, uint16_t address)
{
}

static uint8_t read_mbc_ram(struct cpu *cpu, uint16_t address)
{
}

static void write_mbc_ram(struct cpu *cpu, uint16_t address, uint8_t val)
{
}

static void write_mbc(struct cpu *cpu, uint16_t address, uint8_t val)
{
}
