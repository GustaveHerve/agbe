#include "emulation.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "apu.h"
#include "cpu.h"
#include "disassembler.h"
#include "interrupts.h"
#include "mbc.h"
#include "ppu.h"
#include "serial.h"
#include "timers.h"

#define CYCLE_PER_FRAME 1048576.0f
#define FRAMERATE 59.73f

void init_hardware(struct cpu *cpu)
{
    cpu->membus[0xFF00] = 0xCF;
    cpu->membus[0xFF01] = 0x00;
    cpu->membus[0xFF02] = 0x7E;
    cpu->membus[0xFF04] = 0xAB;
    cpu->internal_div = 0xAB00;
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
    cpu->membus[0xFF50] = 0xFF;
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

    // ppu_reset(cpu->ppu);

    cpu->ppu->current_mode = 1;
    cpu->ppu->line_dot_count = 400;
    cpu->ppu->mode1_153th = 1;
}

void main_loop(struct cpu *cpu, char *rom_path, char *boot_rom_path)
{
    cpu->running = 1;

    if (boot_rom_path != NULL)
    {
        // Enable bootrom
        cpu->membus[0xFF50] = 0xFE;

        // Open BOOTROM
        FILE *fptr = fopen("testroms/boot.gb", "rb");
        if (!fptr)
        {
            fprintf(stderr, "Invalid boot rom path: %s\n", boot_rom_path);
            return;
        }
        fseek(fptr, 0, SEEK_END);
        long fsize = ftell(fptr);
        rewind(fptr);

        fread(cpu->membus, 1, fsize, fptr);
        fclose(fptr);
    }

    // Open ROM, get its size and and copy its content in MBC struct
    FILE *fptr = fopen(rom_path, "rb");
    if (!fptr)
    {
        fprintf(stderr, "Invalid rom path: %s\n", rom_path);
        return;
    }
    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    rewind(fptr);

    uint8_t *rom = malloc(sizeof(uint8_t) * fsize);
    fread(rom, 1, fsize, fptr);
    fclose(fptr);

    uint8_t checksum = rom[0x14d];

    // Init MBC / cartridge info and fill rom in buffer
    set_mbc(cpu, rom);

    lcd_off(cpu);

#if 0
    size_t cycle_threshold = CYCLE_PER_FRAME / FRAMERATE;
    size_t cycle_count = 0;
    Uint64 start = SDL_GetPerformanceCounter();
#endif

    if (boot_rom_path == NULL)
    {
        cpu->regist->pc = 0x0100;
        cpu_init_registers(cpu, checksum);
        init_hardware(cpu);
    }

    while (cpu->running)
    {
#if 0
        if (cycle_count >= cycle_threshold)
        {
            Uint64 end = SDL_GetPerformanceCounter();
            float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
            if (floor(16.666f - elapsedMS > 0))
                SDL_Delay(floor(16.666f - elapsedMS));
            cycle_count -=cycle_threshold;
            start = SDL_GetPerformanceCounter();
        }
#endif
        if (SDL_GetQueuedAudioSize(cpu->apu->device_id) == AUDIO_BUFFER_SIZE * sizeof(float))
        {
            /* Wait for audio queue to be cleared */
            while (SDL_GetQueuedAudioSize(cpu->apu->device_id) > 0)
                continue;
        }

        if (!cpu->halt)
            next_op(cpu);
        // cycle_count += next_op(cpu);
        else
        {
            tick_m(cpu); // Previous instruction tick + next OPCode fetch
            // cycle_count += 1;
        }

        check_interrupt(cpu);
    }
}

void tick_m(struct cpu *cpu)
{
    if (cpu->ime == 2)
        cpu->ime = 1;

    apu_tick_m(cpu->apu);

    update_timers(cpu);
    update_serial(cpu);

    if (get_lcdc(cpu->ppu, LCDC_LCD_PPU_ENABLE))
        ppu_tick_m(cpu->ppu);
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

    // Echo RAM
    else if (address >= 0xE000 && address <= 0xFDFF)
        return cpu->membus[address - 0x2000];

    // OAM
    else if (address >= 0xFE00 && address <= 0xFEFF)
    {
        if (cpu->ppu->oam_locked)
            return 0xFF;
    }

    // JOYP
    else if (address == 0xFF00)
    {
        // Neither directions nor actions buttons selected, low nibble = 0xF
        if ((cpu->membus[address] & 0x30) == 0x30)
            return cpu->membus[address] | 0xF;
    }

    return cpu->membus[address];
}

uint8_t read_mem_tick(struct cpu *cpu, uint16_t address)
{
    uint8_t res = read_mem(cpu, address);
    tick_m(cpu);
    return res;
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

    // Echo RAM
    else if (address >= 0xE000 && address <= 0xFDFF)
        address -= 0x2000;

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
        val &= 0x30; // don't write in bit 3-0 and keep only bit 5-4
        uint8_t low_nibble = 0x00;
        if (((val >> 4) & 0x01) == 0x00)
            low_nibble = cpu->joyp_d;
        else if (((val >> 5) & 0x01) == 0x00)
            low_nibble = cpu->joyp_a;
        else
            low_nibble = 0xF;
        uint8_t new = low_nibble & 0x0F;
        new |= val;
        new |= (cpu->membus[address] & 0xC0); // keep the 7-6 bit
        cpu->membus[address] = new;
    }

    // SC
    else if (address == 0xFF02)
    {
        *cpu->sc = 0x7C | (val & 0x81);
        write = 0;
    }

    // DIV
    else if (address == 0xFF04)
    {
        cpu->internal_div = 0;
        *cpu->div = 0;
        write = 0;
    }

    // TAC
    else if (address == 0xFF07)
    {
        *cpu->tac = 0xF8 | (val & 0x7);
        write = 0;
    }

    // IF
    else if (address == 0xFF0F)
    {
        write = 0;
        uint8_t temp = (cpu->membus[address] & 0xE0);
        temp |= (val & 0x1F);
        cpu->membus[address] = temp;
    }

    // APU registers
    else if (address == NR14 || address == NR24 || address == NR34 || address == NR44)
    {
        write = 0;
        cpu->membus[address] = val & ~(NRx4_UNUSED_PART);
        uint8_t ch_number = ((address - NR14) / (NR24 - NR14)) + 1;
        /* Trigger event */
        if (val & NRx4_TRIGGER_MASK)
        {
            static void (*trigger_handlers[])(struct apu *) = {
                &handle_trigger_event_ch1,
                &handle_trigger_event_ch2,
                &handle_trigger_event_ch3,
                &handle_trigger_event_ch4,
            };

            trigger_handlers[ch_number - 1](cpu->apu);
        }

        if (val & NRx4_LENGTH_ENABLE)
            enable_timer(cpu->apu, ch_number);
    }

    // STAT
    else if (address == 0xFF40)
    {
        // LCD off
        if (!(val >> 6))
            ppu_reset(cpu->ppu);
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

    if (write)
        cpu->membus[address] = val;

    tick_m(cpu);
}
