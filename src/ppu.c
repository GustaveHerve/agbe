#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"
#include "ppu.h"
#include "ppu_utils.h"
#include "emulation.h"
#include "queue.h"

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

void ppu_free(struct ppu *ppu)
{
    queue_free(ppu->bg_fifo);
    queue_free(ppu->obj_fifo);
}

void ppu_tick(struct ppu *ppu)
{
    switch (ppu->current_mode)
    {
        case 2:
            break;
        case 3:
            break;
        case 1:
            break;
        case 0:
            break;
    }
}

//Mode 2
int oam_scan(struct ppu *ppu)
{
    ppu->oam_locked = 1;
    ppu->vram_locked = 0;

    uint8_t *obj_y = ppu->oam + 4 * (ppu->scan_acc);
    if (ppu->obj_count < 10)
    {
        if (*(obj_y + 1) != 0 && *ppu->ly + 16 >= *obj_y
                && *ppu->ly + 16 < *obj_y + 8)
        {
            ppu->obj_slots[ppu->obj_count].y = obj_y;
            ppu->obj_slots[ppu->obj_count].x = obj_y + 1;
            ppu->obj_slots[ppu->obj_count].index = obj_y + 2;
            ppu->obj_slots[ppu->obj_count].attributes = obj_y + 3;
            ppu->obj_count++;
        }
    }
    ppu->scan_acc++;
    if (ppu->scan_acc >= 40)
        ppu->current_mode = 3;

    return 2;
}

int oam_scan_m(struct ppu *ppu)
{
    oam_scan(ppu);
    oam_scan(ppu);
    return 4;
}

//Mode 3
int drawing_pixels(struct ppu *ppu)
{
    //Set read restrictions
    ppu->oam_locked = 1;
    ppu->vram_locked = 1;
    //Clear the queues
    queue_clear(ppu->bg_fifo);
    queue_clear(ppu->obj_fifo);
    int time = 172;

    //Get Tile ID
    //BG or Window Mode ?
    int win_mode = 0;
    //TODO handle obj mode activation
    int obj_mode = 0;
    int obj_index = 0;
    if (!get_lcdc(ppu, 0))
    {
        //TODO BG and Window disabled
    }
    else if (!get_lcdc(ppu, 5))
        win_mode = 0;
    else
    {
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
    tick(ppu->cpu);
    uint8_t tileid = ppu->cpu->membus[address];
    tick(ppu->cpu);

    int bit_12 = 0;
    //Get tile low
    if (obj_mode)
        y_part = (*ppu->ly - *ppu->obj_slots[obj_index].y) % 8;
    else if (win_mode)
    {
        y_part = *ppu->wy % 8;
        bit_12 = !(get_lcdc(ppu, 4) | (tileid & 0x80));
    }
    else
    {
        y_part = (uint8_t)(((*ppu->ly + *ppu->scy)) % 8);
        bit_12 = !(get_lcdc(ppu, 4) | (tileid & 0x80));
    }

    uint16_t address_low = (0x4 << 13) | (bit_12 << 12) |
        (tileid << 4) | (y_part << 1) | 0;

    ppu_tick(ppu);
    uint8_t slice_low = ppu->cpu->membus[address_low];
    ppu_tick(ppu);

    uint16_t address_high = address_low | 1;
    ppu_tick(ppu);
    uint8_t slice_high = ppu->cpu->membus[address_high];
    ppu_tick(ppu);

    return time;
}

uint8_t get_tileid(struct ppu *ppu)
{
    //Get Tile ID
    //BG or Window Mode ?
    int win_mode = 0;
    //TODO handle obj mode activation
    int obj_mode = 0;
    int obj_index = 0;
    if (!get_lcdc(ppu, 0))
    {
        //TODO BG and Window disabled
    }
    else if (!get_lcdc(ppu, 5))
        win_mode = 0;
    else
    {
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

    return tileid;
}

uint8_t get_tile_lo(struct ppu *ppu, uint8_t tileid)
{
    //Get tile low
    int obj_mode = 0;
    int obj_index = 0;
    int win_mode = 0;
    uint8_t y_part = 0;
    int bit_12 = 0;
    if (obj_mode)
        y_part = (*ppu->ly - *ppu->obj_slots[obj_index].y) % 8;
    else if (win_mode)
    {
        y_part = *ppu->wy % 8;
        bit_12 = !(get_lcdc(ppu, 4) | (tileid & 0x80));
    }
    else
    {
        y_part = (uint8_t)(((*ppu->ly + *ppu->scy)) % 8);
        bit_12 = !(get_lcdc(ppu, 4) | (tileid & 0x80));
    }

    uint16_t address_low = (0x4 << 13) | (bit_12 << 12) |
        (tileid << 4) | (y_part << 1) | 0;

    uint8_t slice_low = ppu->cpu->membus[address_low];

    return slice_low;
}

uint8_t get_tile_hi(struct ppu *ppu, uint8_t tileid)
{
    //Get tile low
    int obj_mode = 0;
    int obj_index = 0;
    int win_mode = 0;
    uint8_t y_part = 0;
    int bit_12 = 0;
    if (obj_mode)
        y_part = (*ppu->ly - *ppu->obj_slots[obj_index].y) % 8;
    else if (win_mode)
    {
        y_part = *ppu->wy % 8;
        bit_12 = !(get_lcdc(ppu, 4) | (tileid & 0x80));
    }
    else
    {
        y_part = (uint8_t)(((*ppu->ly + *ppu->scy)) % 8);
        bit_12 = !(get_lcdc(ppu, 4) | (tileid & 0x80));
    }

    uint16_t address_high = (0x4 << 13) | (bit_12 << 12) |
        (tileid << 4) | (y_part << 1) | 1;

    uint8_t slice_high = ppu->cpu->membus[address_high];

    return slice_high;
}

int push_pixel(struct queue *target, struct pixel p)
{
    queue_push(target, p);
    return 1;
}

//Mode 0 Horizontal Blank
//PPU sleeps for time dots
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

//in_object: read LX and LY and check if drawing a selected object
//returns object index in obj_slots, -1 if no object
int in_object(struct ppu *ppu)
{
    for (int i = 0; i < ppu->obj_count; i++)
    {
        if (*ppu->obj_slots[i].x == ppu->lx + 8 &&
            *ppu->ly + 16 >= *ppu->obj_slots[i].y &&
            *ppu->ly + 16 < *ppu->obj_slots[i].y + 8)
            return i;
    }
    return -1;
}
