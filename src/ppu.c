#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"
#include "ppu.h"
#include "ppu_utils.h"
#include "emulation.h"

void ppu_init(struct ppu *ppu, struct cpu *cpu)
{
    ppu->cpu = cpu;
    cpu->ppu = ppu;

    ppu->oam = cpu->membus + 0xFE00;
    ppu->lcdc = cpu->membus + 0xFF40;
    ppu->lx = 0;
    ppu->ly = cpu->membus + 0xFF44;
    ppu->lyc = cpu->membus + 0xFF45;
    ppu->scy = cpu->membus + 0xFF42;
    ppu->scx = cpu->membus + 0xFF43;
    ppu->wy = cpu->membus + 0xFF4A;
    ppu->wx = cpu->membus + 0xFF4B;
    ppu->stat = cpu->membus + 0xFF41;

    ppu->bg_fifo = queue_init();
    ppu->obj_fifo = queue_init();
}

void ppu_tick(struct ppu *ppu)
{
    return;
}

//Mode 2
int oam_scan(struct ppu *ppu)
{
    ppu->oam_locked = 1;
    ppu->vram_locked = 0;
    uint8_t *obj_y = ppu->oam;
    int acc = 0;
    for (int i = 0; i < 40; i++)
    {
        int tempy = (int) *obj_y;
        tempy -= 16;
        if (acc < 10 && *ppu->ly - tempy < 8)
        {
            ppu->obj_slots[acc].y = obj_y;
            ppu->obj_slots[acc].x = obj_y + 1;
            ppu->obj_slots[acc].index = obj_y + 2;
            ppu->obj_slots[acc].attributes = obj_y + 3;
            acc++;
        }
        obj_y += 4;
        tick(ppu->cpu); //TODO verify tick timing
        tick(ppu->cpu);
    }
    return 80; //Duration of 80 dots always
}

//Mode 3
int drawing_pixels(struct ppu *ppu)
{
    ppu->oam_locked = 1;
    ppu->vram_locked = 1;
    queue_clear(ppu->bg_fifo);
    queue_clear(ppu->obj_fifo);
    int time = 172;
    //BG or Window Mode ?
    int win_mode = 0;
    if (!get_lcdc(ppu, 0))
    {
        //TODO BG and Window disabled
    }
    else if (!get_lcdc(ppu, 5))
        win_mode = 0;
    else
    {
        //TODO verify if we are in window coordinates
        if (in_window(ppu))
            win_mode = 1;
        else
            win_mode = 0;
    }

    uint8_t x_part = 0;
    uint8_t y_part = 0;
    int bit = 0;
    if (win_mode)
    {
        x_part = ppu->lx / 8;
        y_part = *ppu->wy / 8;
        bit = 6;
    }
    else
    {
        x_part =  ((uint8_t) (ppu->lx + *ppu->scx)) / 8;
        y_part =  ((uint8_t) (*ppu->ly + *ppu->scy)) / 8;
        bit = 3;
    }

    uint16_t address = (0x13 << 11) | (get_lcdc(ppu, bit) << 10)
        | (y_part << 5) | x_part;

    uint8_t tileid = ppu->cpu->membus[address];

    return time;
}

//Mode 0
int hblank(struct ppu *ppu, int prev_time)
{
    int time = 456 - prev_time;
    ppu->oam_locked = 0;
    ppu->vram_locked = 0;
    for (int i = 0; i < time; i++)
        tick(ppu->cpu);
    return time;
}

int get_lcdc(struct ppu *ppu, int bit)
{
    uint8_t lcdc = *ppu->lcdc;
    return (lcdc >> bit & 0x01);
}

//in_window: read LX and LY and check if drawing in Window or BG
int in_window(struct ppu *ppu)
{
    return *ppu->ly >= *ppu->wy && ppu->lx >= *ppu->wx;
}
