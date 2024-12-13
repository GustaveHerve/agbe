#include "mbc3.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "save.h"

static void _mbc_free(struct mbc_base *mbc)
{
    (void)mbc;
}

static uint8_t _read_mbc_rom(struct cpu *cpu, uint16_t address)
{
    struct mbc3 *mbc = (struct mbc3 *)cpu->mbc;

    unsigned int res_addr = address & 0x3FFF;
    if (address >= 0x4000 && address <= 0x7FFF)
        res_addr = (mbc->bank1 << 14) | res_addr;

    // If address is too big for the size of ROM, only keep necessary bit count
    // to address total ROM size values
    unsigned int mask = 0x00100000;
    while (res_addr > cpu->mbc->rom_total_size)
    {
        res_addr &= (~mask);
        mask >>= 1;
    }

    return cpu->mbc->rom[res_addr];
}

static void _write_mbc_rom(struct cpu *cpu, uint16_t address, uint8_t val)
{
    struct mbc3 *mbc = (struct mbc3 *)cpu->mbc;

    // RAM Enable
    if (address <= 0x1FFF)
    {
        if ((val & 0x0F) == 0x0A)
            mbc->ram_rtc_registers_enabled = 1;
        else
            mbc->ram_rtc_registers_enabled = 0;
    }

    else if (address >= 0x2000 && address <= 0x3FFF)
    {
        uint8_t bank = val & 0x7F;
        // Prevent bank 0x00 duplication (only if uses 7 bits)
        if (bank == 0x00)
            mbc->bank1 = 0x01;
        else
        {
            uint8_t mask = cpu->mbc->rom_bank_count - 1;
            mbc->bank1 = bank & mask;
        }
    }

    // RAM bank switch OR RTC register select
    else if (address >= 0x4000 && address <= 0x5FFF)
    {
        if (val < 0x08 || val > 0x0C)
            mbc->bank2 = val & 0x03;
    }

    // Latch Clock Data
    else if (address >= 0x6000 && address <= 0x7FFF)
    {
        if (mbc->latch_last_write == 0x00 && val == 0x01)
        {
            // Latch RTC registers
        }
        mbc->latch_last_write = val;
    }
}

static uint8_t _read_mbc_ram(struct cpu *cpu, uint16_t address)
{
    struct mbc3 *mbc = (struct mbc3 *)cpu->mbc;

    if (!mbc->ram_rtc_registers_enabled || cpu->mbc->ram_bank_count == 0)
        return 0xFF;

    // RTC register mapping
    switch (mbc->bank2)
    {
    case 0x08:
        return mbc->rtc_clock.s;
    case 0x09:
        return mbc->rtc_clock.m;
    case 0x0A:
        return mbc->rtc_clock.h;
    case 0x0B:
        return mbc->rtc_clock.dl;
    case 0x0C:
        return mbc->rtc_clock.dh;
    }

    unsigned int res_addr = address & 0x1FFF;
    // RAM bank mapping
    res_addr = (mbc->bank2 << 13) | res_addr;

    // If address is too big for the size of RAM, ignore as many bits as needed
    unsigned int mask = 0x4000;
    while (res_addr > cpu->mbc->ram_total_size)
    {
        res_addr &= ~mask;
        mask >>= 1;
    }
    return cpu->mbc->ram[res_addr];
}

static void _write_mbc_ram(struct cpu *cpu, uint16_t address, uint8_t val)
{
    struct mbc3 *mbc = (struct mbc3 *)cpu->mbc;

    // Ignore writes if RAM is disabled or if there is no external RAM
    if (!mbc->ram_rtc_registers_enabled || cpu->mbc->ram_bank_count == 0)
        return;

    unsigned int res_addr = address & 0x1FFF;
    if (mbc->mbc1_mode)
        res_addr = (mbc->bank2 << 13) | res_addr;

    // If address is too big for the size of RAM, ignore as many bits as needed
    unsigned int mask = 0x4000;
    while (res_addr > cpu->mbc->ram_total_size)
    {
        res_addr &= (~mask);
        mask >>= 1;
    }

    cpu->mbc->ram[res_addr] = val;

    // Save if MBC has a save battery
    if (cpu->mbc != NULL)
        save_ram_to_file(cpu->mbc);
}

struct mbc_base *make_mbc3(void)
{
    struct mbc_base *mbc = calloc(1, sizeof(struct mbc3));
    struct mbc3 *mbc3 = (struct mbc3 *)mbc;

    mbc->type = MBC3;

    mbc->_mbc_free = &_mbc_free;

    mbc->_read_mbc_rom = &_read_mbc_rom;
    mbc->_write_mbc_rom = &_write_mbc_rom;

    mbc->_read_mbc_ram = &_read_mbc_ram;
    mbc->_write_mbc_ram = &_write_mbc_ram;

    mbc3->bank1 = 1;
    mbc3->bank2 = 0;
    mbc3->ram_rtc_registers_enabled = 0;

    mbc3->rtc_clock.s = 0;
    mbc3->rtc_clock.m = 0;
    mbc3->rtc_clock.h = 0;
    mbc3->rtc_clock.dl = 0;
    mbc3->rtc_clock.dh = 0;

    return mbc;
}
