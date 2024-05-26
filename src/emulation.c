#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <err.h>
#include "cpu.h"
#include "ppu.h"
#include "utils.h"
#include "disassembler.h"
#include "emulation.h"
#include "mbc.h"
#include "serial.h"

#define CYCLE_PER_FRAME 1048576.0f
#define FRAMERATE 60.0f

void init_cpu(struct cpu *cpu, int checksum)
{
    cpu->regist->a = 0x01;
    set_z(cpu->regist, 1);
    set_n(cpu->regist, 0);
    if (checksum == 0x00)
    {
        set_h(cpu->regist, 0);
        set_c(cpu->regist, 0);
    }
    else
    {
        set_h(cpu->regist, 1);
        set_c(cpu->regist, 1);
    }
    cpu->regist->b = 0x00;
    cpu->regist->c = 0x13;
    cpu->regist->d = 0x00;
    cpu->regist->e = 0xD8;
    cpu->regist->h = 0x01;
    cpu->regist->l = 0x4D;
    cpu->regist->pc = 0x0100;
    cpu->regist->sp = 0xFFFE;
}

void init_hardware(struct cpu *cpu)
{
    cpu->membus[0xFF00] = 0xCF;
    cpu->membus[0xFF01] = 0x00;
    cpu->membus[0xFF02] = 0x7E;
    cpu->membus[0xFF04] = 0xAB;
    cpu->membus[0xFF05] = 0x00;
    cpu->membus[0xFF06] = 0x00;
    cpu->membus[0xFF07] = 0xF8;
    cpu->membus[0xFF0F] = 0xE1;
    cpu->membus[0xFF10] = 0x80;
    cpu->membus[0xFF11] = 0xBF;
    cpu->membus[0xFF12] = 0xF3;
    cpu->membus[0xFF13] = 0xFF;
    cpu->membus[0xFF14] = 0xBF;
    cpu->membus[0xFF16] = 0x3F;
    cpu->membus[0xFF17] = 0x00;
    cpu->membus[0xFF18] = 0xFF;
    cpu->membus[0xFF19] = 0xBF;
    cpu->membus[0xFF1A] = 0x7F;
    cpu->membus[0xFF1B] = 0xFF;
    cpu->membus[0xFF1C] = 0x9F;
    cpu->membus[0xFF1D] = 0xFF;
    cpu->membus[0xFF1E] = 0xBF;
    cpu->membus[0xFF20] = 0xFF;
    cpu->membus[0xFF21] = 0x00;
    cpu->membus[0xFF22] = 0x00;
    cpu->membus[0xFF23] = 0xBF;
    cpu->membus[0xFF24] = 0x77;
    cpu->membus[0xFF25] = 0xF3;
    cpu->membus[0xFF26] = 0xF1;
    cpu->membus[0xFF40] = 0x91;
    cpu->membus[0xFF41] = 0x85;
    cpu->membus[0xFF42] = 0x00;
    cpu->membus[0xFF43] = 0x00;
    cpu->membus[0xFF44] = 0x00;
    cpu->membus[0xFF45] = 0x00;
    cpu->membus[0xFF46] = 0xFF;
    cpu->membus[0xFF47] = 0xFC;
    cpu->membus[0xFF48] = 0xFF;
    cpu->membus[0xFF49] = 0xFF;
    cpu->membus[0xFF4A] = 0x00;
    cpu->membus[0xFF4B] = 0x00;
    cpu->membus[0xFF4D] = 0xFF;
    cpu->membus[0xFF4F] = 0xFF;
    //cpu->membus[0xFF50] = 0xFE;
    cpu->membus[0xFF51] = 0xFF;
    cpu->membus[0xFF52] = 0xFF;
    cpu->membus[0xFF53] = 0xFF;
    cpu->membus[0xFF54] = 0xFF;
    cpu->membus[0xFF55] = 0xFF;
    cpu->membus[0xFF56] = 0xFF;
    cpu->membus[0xFF68] = 0xFF;
    cpu->membus[0xFF69] = 0xFF;
    cpu->membus[0xFF6A] = 0xFF;
    cpu->membus[0xFF6B] = 0xFF;
    cpu->membus[0xFF70] = 0xFF;
    cpu->membus[0xFFFF] = 0x00;

    ppu_reset(cpu->ppu);
}

void main_loop(struct cpu *cpu, char *rom_path)
{
    cpu->running = 1;

    // Enable bootrom
    cpu->membus[0xFF50] = 0xFE;

    // Open BOOTROM
    FILE *fptr = fopen("testroms/boot.gb", "rb");
    if (!fptr)
    {
        puts("Unable to find boot.gb");
        return;
    }
    fread(cpu->membus, 1, 256, fptr);
    fclose(fptr);

    // Open ROM, get its size and and copy its content in MBC struct
    fptr = fopen(rom_path, "rb");
    if (!fptr)
    {
        puts("Unable to find rom");
        return;
    }
    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    rewind(fptr);

    uint8_t *rom = malloc(sizeof(uint8_t) * fsize);
    fread(rom, 1, fsize, fptr);
    fclose(fptr);

    // Init MBC / cartridge info and fill rom in buffer
    set_mbc(cpu, rom);

    lcd_off(cpu);

    size_t cycle_threshold = CYCLE_PER_FRAME / FRAMERATE;
    size_t cycle_count = 0;
    Uint64 last_ticks = SDL_GetTicks64();
    while (cpu->running && cpu->regist->pc != 0x0150)
    {
        if (cycle_count >= cycle_threshold)
        {
            Uint64 current_tick = SDL_GetTicks64();
            if (current_tick - last_ticks < 1000.0f / FRAMERATE)
                continue;
            cycle_count -=cycle_threshold;
            last_ticks = current_tick;

        }
        //TODO handle halt state
        cycle_count += next_op(cpu); // Remaining MCycles are ticked in instructions
        check_interrupt(cpu);
    }

    //init_cpu(cpu, 0x0a);
    init_hardware(cpu);
    cpu->div_timer = 52;

    while (cpu->running)
    {
        if (cycle_count >= cycle_threshold)
        {
            Uint64 current_tick = SDL_GetTicks64();
            if (current_tick - last_ticks < 1000.0f / FRAMERATE)
                continue;
            cycle_count -=cycle_threshold;
            last_ticks = current_tick;
        }

        if (!cpu->halt)
            cycle_count += next_op(cpu);
        else
        {
            tick_m(cpu); // Previous instruction tick + next OPCode fetch
            cycle_count += 1;
        }

        check_interrupt(cpu);
    }
}

void tick_m(struct cpu *cpu)
{
    if (cpu->ime == 2)
        cpu->ime = 1;

    if (!cpu->stop)
        ++cpu->div_timer;

    if (cpu->div_timer >= 64)
    {
        *cpu->div += 1;
        cpu->div_timer = 0;
    }

    uint8_t previous = *cpu->tima;
    if (*cpu->tac >> 2 & 0x01)
    {
        int temp = 0;
        uint8_t clock = *cpu->tac & 0x03;
        switch (clock)
        {
            case 0:
                temp = 256;
                break;
            case 1:
                temp = 4;
                break;
            case 2:
                temp = 16;
                break;
            case 3:
                temp = 64;
                break;
        }


        if (cpu->tima_timer >= temp)
        {
            *cpu->tima += 1;
            cpu->tima_timer = 0;
        }
    }

    // Overflow
    if (previous > *cpu->tima)
    {
        *cpu->tima = *cpu->tma;
        set_if(cpu, 2);
    }

    if (get_lcdc(cpu->ppu, 7))
        ppu_tick_m(cpu->ppu);
    else
    {
        cpu->ppu->oam_locked = 0;
        cpu->ppu->vram_locked = 0;
        ppu_reset(cpu->ppu);
    }

    // Serial clock
    cpu->serial_clock += 4;
    if (get_transfer_enable(cpu) && get_clock_select(cpu) && cpu->serial_clock == 512)
    {
        if (cpu->serial_acc == 0)
            cpu->serial_acc = 8;
        serial_transfer(cpu);
        --cpu->serial_acc;
        if (cpu->serial_acc == 0)
            set_if(cpu, 3);
        cpu->serial_clock = 0;
    }
}

uint8_t read_mem(struct cpu *cpu, uint16_t address)
{
    // DMA, can only access HRAM
    /*
    if (cpu->ppu->dma == 1 && (address < 0xFF80 || address > 0xFFFE))
    {
        return 0xFF;
    }
    */

    // BOOTROM mapping
    if (!(*cpu->boot & 0x01) && address <= 0x00FF)
    {
        tick_m(cpu);
        return cpu->membus[address];
    }

    // ROM
    else if (address <= 0x7FFF)
    {
        tick_m(cpu);
        return read_mbc_rom(cpu, address);
    }

    // VRAM
    else if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (cpu->ppu->vram_locked)
            return 0xFF;
    }

    // External RAM read
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        tick_m(cpu);
        return read_mbc_ram(cpu, address);
    }

    // OAM
    else if (address >= 0xFE00 && address <= 0xFEFF)
    {
        if (cpu->ppu->oam_locked)
            return 0xFF;
    }

    tick_m(cpu);
    return cpu->membus[address];
}

void write_mem(struct cpu *cpu, uint16_t address, uint8_t val)
{
    uint8_t write = 1;
    if (address <= 0x7FFF)
    {
        write = 0;
        write_mbc(cpu, address, val);
    }

    // VRAM
    else if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (cpu->ppu->vram_locked)
            write = 0;
    }

    // External RAM
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        write = 0;
        write_mbc(cpu, address, val);
    }

    // OAM
    else if (address >= 0xFE00 && address <= 0xFEFF)
    {
        if (cpu->ppu->oam_locked)
            write = 0;
    }

    // JOYP
    else if (address == 0xFF00)
    {
        write = 0;
        val &= 0x30; // don't write in bit 3-0 and keep only bit 6-5
        uint8_t low_nibble = 0x00;
        if (((val >> 4) & 0x01) == 0x00)
            low_nibble = cpu->joyp_d;
        if (((val >> 5) & 0x01) == 0x00)
            low_nibble = cpu->joyp_a;
        uint8_t new = low_nibble & 0x0F;
        new |= val;
        new |= (cpu->membus[address] & 0xC0); //keep the 7-6 bit
        cpu->membus[address] = new;
    }

    // DIV
    else if (address == 0xFF04)
        *cpu->div = 0;

    // IF
    else if (address == 0xFF0F)
    {
        write = 0;
        uint8_t temp = (cpu->membus[address] & 0xE0);
        temp |= (val & 0x1F);
        cpu->membus[address] = temp;
    }

    // DMA
    else if (address == 0xFF46)
    {
        write = 0;
        cpu->ppu->dma = 2;
        cpu->ppu->dma_acc = 0;
        cpu->ppu->dma_source = val;
    }
    
    // BOOT
    else if (address == 0xFF50)
    {
        // Prevent enabling bootrom again
        if (cpu->membus[0xFF50] & 0x01)
            write = 0;
    }

    // IE
    else if (address == 0xFFFF)
    {
        write = 0;
        uint8_t temp = (cpu->membus[address] & 0xE0);
        temp |= (val & 0x1F);
        cpu->membus[address] = temp;
    }

    tick_m(cpu);

    if (write)
        cpu->membus[address] = val;
}

uint8_t read_mem_no_tick(struct cpu *cpu, uint16_t address)
{
    // BOOTROM mapping
    if (!(*cpu->boot & 0x01) && address <= 0x00FF)
        return cpu->membus[address];

    // ROM
    else if (address <= 0x7FFF)
        return read_mbc_rom(cpu, address);

    // VRAM
    else if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (cpu->ppu->vram_locked)
            return 0xFF;
    }

    // External RAM read
    else if (address >= 0xA000 && address <= 0xBFFF)
        return read_mbc_ram(cpu, address);

    // OAM
    else if (address >= 0xFE00 && address <= 0xFEFF)
    {
        if (cpu->ppu->oam_locked)
            return 0xFF;
    }

    return cpu->membus[address];
}
