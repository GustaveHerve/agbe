#include <stdlib.h>
#include <err.h>
#include "cpu.h"
#include "utils.h"
#include "ppu.h"
#include "ppu_utils.h"
#include "emulation.h"
#include "queue.h"
#include "rendering.h"

void ppu_init(struct ppu *ppu, struct cpu *cpu, struct renderer *renderer)
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

    ppu->bgp = cpu->membus + 0xFF47;
    ppu->obp0 = cpu->membus + 0xFF48;
    ppu->obp1 = cpu->membus + 0xFF49;

    ppu->obj_count = 0;
    ppu->has_pushed = 0;

    ppu->bg_fifo = queue_init();
    ppu->obj_fifo = queue_init();

    ppu->bg_fetcher = malloc(sizeof(fetcher));
    ppu->obj_fetcher = malloc(sizeof(fetcher));

    fetcher_init(ppu->bg_fetcher);
    fetcher_init(ppu->obj_fetcher);

    ppu->renderer = renderer;

    ppu->current_mode = 0;
    ppu->oam_locked = 0;
    ppu->vram_locked = 0;

    ppu->dma = 0;
    ppu->dma_acc = 0;

    ppu->line_dot_count = 0;
    ppu->mode1_153th = 0;

    ppu->win_mode = 0;
    ppu->pop_pause = 0;

    *ppu->lcdc = 0x00;
    *ppu->stat = 0x84;
    *ppu->scy = 0x00;
    *ppu->scx = 0x00;
    *ppu->ly = 0x00;
    *ppu->lyc = 0x00;
    //*ppu->dma = 0xFF
    *ppu->bgp = 0xFC;
    *ppu->obp0 = 0xFF;
    *ppu->obp1 = 0xFF;
    *ppu->wx = 0x00;
    *ppu->wy = 0x00;
}

void ppu_free(struct ppu *ppu)
{
    free(ppu->bg_fetcher);
    free(ppu->obj_fetcher);
    queue_free(ppu->bg_fifo);
    queue_free(ppu->obj_fifo);
    free(ppu);
}

//Sets back PPU to default state when turned off
void ppu_reset(struct ppu *ppu)
{
    ppu->current_mode = 1;
    ppu->line_dot_count = 400;
    ppu->mode1_153th = 1;
}

//Tick 4 dots
void ppu_tick_m(struct ppu *ppu)
{
    int dots = 4;
    while (dots > 0)
    {
        switch (ppu->current_mode)
        {
            case 2: //Mode 2 - OAM scan
            {
                oam_scan(ppu);
                dots -= 2;
                break;
            }
            case 3: //Mode 3 - Drawing pixels and FIFOs fetcher activity
            {
                ppu->oam_locked = 1;
                ppu->vram_locked = 1;

                int time = 0;
                //reset FIFOs at beginning of Mode 3
                if (ppu->line_dot_count == 80)
                {
                    ppu->pop_pause = 0;
                    queue_clear(ppu->bg_fifo);
                    queue_clear(ppu->obj_fifo);
                }
                //End of line, go to HBlank
                else if (ppu->lx > 167)
                {
                    ppu->current_mode = 0;
                    clear_stat(ppu, 1);
                    clear_stat(ppu, 0);
                    if (get_stat(ppu, 3) && !get_if(ppu->cpu, 1))
                        set_if(ppu->cpu, 1);
                    break;
                }

                int obj = -1;
                if (get_lcdc(ppu, 1))
                    obj = in_object(ppu, ppu->obj_fetcher->obj_index);

                //Check if in Window and WIN Enable
                if (!ppu->win_mode && get_lcdc(ppu, 0) && get_lcdc(ppu, 5) && in_window(ppu)) //Win mode -> clear + reset BG FIFO
                {
                    ppu->win_mode = 1;
                    queue_clear(ppu->bg_fifo);
                    ppu->bg_fetcher->current_step = 0;
                }
                //Check if there is an OBJ on current LX
                else if (obj != -1)
                {
                    ppu->pop_pause = 1;
                }
                else
                    time = bg_fetcher_step(ppu);


                //FIFOs popping is paused if OBJ was detected
                if (!ppu->pop_pause && !queue_isempty(ppu->bg_fifo))
                {
                    //TODO select pixel if obj fifo not empty
                    //TODO insert time x pixel rendered (1 dot = 1 pixel pop)
                    if (time == 0)
                        time = 1;
                    for (int i = 0; i < time; i++)
                    {
                        struct pixel p = select_pixel(ppu);
                        if (ppu->lx > 7) //Don't draw prefetch
                            draw_pixel(ppu->cpu, p);
                        ppu->lx++;
                    }
                }
                else if (ppu->pop_pause)
                {
                    //If hasn't pushed gone at step push yet
                    if (!ppu->has_pushed && ppu->bg_fetcher->current_step < 3)
                        time = bg_fetcher_step(ppu);
                    //try to push in case BG FIFO is empty
                    else
                    {
                        bg_fetcher_step(ppu);
                        ppu->has_pushed = 1;
                        time = obj_fetcher_step(ppu);
                    }
                }

                dots -= time;
                ppu->line_dot_count += time;
                break;
            }
            case 0: //Mode 0 - HBlank
            {
                ppu->obj_fetcher->obj_index = -1;
                ppu->oam_locked = 0;
                ppu->vram_locked = 0;
                //TODO verify dma lock
                if (ppu->line_dot_count < 456)
                {
                    ppu->line_dot_count++;
                    dots--;
                }
                else
                {
                    //Exit HBlank
                    ppu->lx = 0;
                    *ppu->ly += 1;
                    ppu->line_dot_count = 0;
                    ppu->current_mode = 2;
                    ppu->obj_count = 0;
                    set_stat(ppu, 1);
                    clear_stat(ppu, 0);
                    if (get_stat(ppu, 5) && !get_stat(ppu, 4))
                        set_if(ppu->cpu, 1);
                    //Start VBlank
                    if (*ppu->ly > 143)
                    {
                        clear_stat(ppu, 1);
                        set_stat(ppu, 0);
                        set_if(ppu->cpu, 0); //Interrupt VBlank
                        if (get_stat(ppu, 4) && !get_stat(ppu, 3))
                            set_if(ppu->cpu, 1);
                        ppu->current_mode = 1;
                        ppu->mode1_153th = 0;
                    }
                }
                break;
            }
            case 1: //Mode 1 - VBlank
            {
                if (ppu->line_dot_count < 456)
                {
                    //LY = 153, special case with LY = 0 after 1 MCycle
                    if (*ppu->ly == 153 && ppu->line_dot_count == 4)
                    {
                        *ppu->ly = 0;
                        ppu->mode1_153th = 1;
                    }
                    ppu->line_dot_count++;
                    dots--;
                }
                else if (!ppu->mode1_153th && *ppu->ly < 153)    //Go to next VBlank line
                {
                    *ppu->ly += 1;
                    if (*ppu->ly == *ppu->lyc)
                        set_stat(ppu, 2);
                    else
                        clear_stat(ppu, 2);
                    ppu->line_dot_count = 0;
                }
                else    //Exit VBlank, enter OAM Scan
                {
                    ppu->line_dot_count = 0;
                    ppu->mode1_153th = 0;
                    ppu->current_mode = 2;
                    *ppu->ly = 0;
                    //TODO move this to more accurate check place
                    if (*ppu->ly == *ppu->lyc)
                    {
                        set_stat(ppu, 2);
                        if (get_stat(ppu, 6)) //&& !get_if(ppu->cpu, 1))
                            set_if(ppu->cpu, 1);
                    }
                    else
                        clear_stat(ppu, 2);
                    set_stat(ppu, 1);
                    clear_stat(ppu, 0);
                }
                break;
            }
        }
    }
    //DMA handling
    //DMA first setup MCycle
    if (ppu->dma == 2)
        ppu->dma = 1;
    else if (ppu->dma == 1)
    {
        ppu->cpu->membus[0xFE00 + ppu->dma_acc] =
            ppu->cpu->membus[(ppu->dma_source << 8) + ppu->dma_acc];
        ppu->dma_acc++;
        if (ppu->dma_acc >= 160)
            ppu->dma = 0;
    }
}

//Mode 2
int oam_scan(struct ppu *ppu)
{
    ppu->oam_locked = 1;
    ppu->vram_locked = 0;

    uint8_t *obj_y = ppu->oam + 2 * (ppu->line_dot_count);
    if (ppu->obj_count < 10)
    {
        if (*(obj_y + 1) != 0 && *ppu->ly + 16 >= *obj_y
                && *ppu->ly + 16 < *obj_y + 8)
        {
            ppu->obj_slots[ppu->obj_count].y = *obj_y;
            ppu->obj_slots[ppu->obj_count].x = *(obj_y + 1);
            ppu->obj_slots[ppu->obj_count].oam_address = obj_y;
            ppu->obj_count++;
        }
    }

    ppu->line_dot_count += 2;
    if (ppu->line_dot_count >= 80)
    {
        ppu->current_mode = 3;
        set_stat(ppu, 1);
        set_stat(ppu, 0);
    }

    return 2;
}

int oam_scan_m(struct ppu *ppu)
{
    oam_scan(ppu);
    oam_scan(ppu);
    return 4;
}

//obj_index == -1 means BG/Win Mode
uint8_t get_tileid(struct ppu *ppu, int obj_index)
{
    uint8_t tileid = 0;
    if (obj_index == -1)
    {
        uint8_t x_part = 0;
        uint8_t y_part = 0;
        int bit = 0;
        if (ppu->win_mode)
        {
            //x_part = (ppu->lx + 8) / 8;
            x_part = (ppu->lx) / 8;
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
        tileid = ppu->cpu->membus[address];
    }

    else
    {
        if (ppu->dma)
            tileid = 0xFF;
        else
            tileid = *(ppu->obj_slots[obj_index].oam_address + 2);
    }

    return tileid;
}

uint8_t get_tile_lo(struct ppu *ppu, uint8_t tileid, int obj_index)
{
    uint8_t y_part = 0;
    int bit_12 = 0;
    if (obj_index != -1)
    {
        y_part = (*ppu->ly - (ppu->obj_slots[obj_index].y - 16)) % 8;
        uint8_t attributes = *(ppu->obj_slots[obj_index].oam_address + 3);
        //Y flip
        if ((attributes >> 6) & 0x01)
        {
            uint8_t temp = 0x00;
            temp = temp | (((y_part >> 1) & 0x01) << 3);
            temp = temp | (((y_part >> 2) & 0x01) << 2);
            temp = temp | (((y_part >> 3) & 0x01) << 1);
            y_part = temp;
        }
    }
    else if (ppu->win_mode)
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

    if (obj_index != -1)
    {
        uint8_t attributes = *(ppu->obj_slots[obj_index].oam_address + 2);
        //X flip
        if ((attributes >> 5) & 0x01)
        {
            slice_low = slice_xflip(slice_low);
        }
    }

    return slice_low;
}

//TODO optimize this ? (address is same as low + 1)
uint8_t get_tile_hi(struct ppu *ppu, uint8_t tileid, int obj_index)
{
    uint8_t y_part = 0;
    int bit_12 = 0;
    if (obj_index != -1)
    {
        y_part = (*ppu->ly - (ppu->obj_slots[obj_index].y - 16)) % 8;
        uint8_t attributes = *(ppu->obj_slots[obj_index].oam_address + 3);
        //Y flip
        if ((attributes >> 6) & 0x01)
        {
            uint8_t temp = 0x00;
            temp = temp | (((y_part >> 1) & 0x01) << 3);
            temp = temp | (((y_part >> 2) & 0x01) << 2);
            temp = temp | (((y_part >> 3) & 0x01) << 1);
            y_part = temp;
        }
    }
    else if (ppu->win_mode)
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

    if (obj_index != -1)
    {
        uint8_t attributes = *(ppu->obj_slots[obj_index].oam_address + 2);
        //X flip
        if ((attributes >> 5) & 0x01)
        {
            slice_high = slice_xflip(slice_high);
        }
    }

    return slice_high;
}

int push_pixel(queue *target, struct pixel p)
{
    queue_push(target, p);
    return 1;
}

int push_slice(struct ppu *ppu, queue *q, uint8_t hi, uint8_t lo, int obj_i)
{
    uint8_t *attributes = NULL;
    if (obj_i != -1)
        attributes = ppu->obj_slots[obj_i].oam_address + 2;
    for (int i = 0; i < 8; i++)
    {
        struct pixel p = make_pixel(hi, lo, i, attributes);
        //TODO verify this
        if (!get_lcdc(ppu, 0))
            p.color = 0;
        push_pixel(q, p);
    }
    return 2;
}

//OBJ Merge version of push_slice in case it is not empty, overwrite transparent pixels OBJ FIFO
int merge_obj(struct ppu *ppu, uint8_t hi, uint8_t lo, int obj_i)
{
    uint8_t *attributes = ppu->obj_slots[obj_i].oam_address + 2;
    queue_node *q = ppu->obj_fifo->front;
    int i = 0;
    while (q != NULL && i < 8)
    {
        //replace transparent pixels
        struct pixel p = make_pixel(hi, lo, i, attributes);
        if (q->data.color == 0)
            q->data = p;
        q = q->next;
        i++;
    }
    return 2; //not sure
}

struct pixel pop_pixel(struct ppu *ppu, int obj)
{
    struct pixel p;
    if (obj)
        p = queue_pop(ppu->obj_fifo);
    else
        p = queue_pop(ppu->bg_fifo);
    return p;
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

//in_object: reads the 10 OAM slots starting from obj_index and checks if we need to draw an object at current LX LY
//returns object index in obj_slots, -1 if no object
int in_object(struct ppu *ppu, int obj_index)
{
    //TODO handle 8x16 object mode
    for (int i = obj_index + 1; i < ppu->obj_count; i++)
    {
        if (ppu->obj_slots[i].x == ppu->lx &&
            *ppu->ly + 16 >= ppu->obj_slots[i].y &&
            *ppu->ly + 16 < ppu->obj_slots[i].y + 8)
        {
            ppu->obj_fetcher->obj_index = i;
            return i;
        }
    }
    return -1;
}


//Fetcher functions
void fetcher_init(fetcher *f)
{
    f->current_step = 0;
    f->hi = 0;
    f->lo = 0;
    f->tileid = 0;
    f->obj_index = -1;
}

//Does one fetcher step (2 dots)
int bg_fetcher_step(struct ppu *ppu)
{
    fetcher *f = ppu->bg_fetcher;
    //BG/Win fetcher
    switch (f->current_step)
    {
        case 0:
            f->tileid = get_tileid(ppu, -1);
            f->current_step = 1;
            break;
        case 1:
            f->lo = get_tile_lo(ppu, f->tileid, -1);
            f->current_step = 2;
            break;
        case 2:
            f->hi = get_tile_hi(ppu, f->tileid, -1);
            f->current_step = 3;
            break;
        case 3:
            {
                //If BG empty, refill it
                if (queue_isempty(ppu->bg_fifo))
                {
                    push_slice(ppu, ppu->bg_fifo, f->hi, f->lo, -1);
                    f->current_step = 0;
                }
                return 0;
            }
    }
    return 2;
}

int obj_fetcher_step(struct ppu *ppu)
{
    fetcher *f = ppu->obj_fetcher;
    switch (f->current_step)
    {
        case 0:
            f->tileid = get_tileid(ppu, f->obj_index);
            f->current_step = 1;
            break;
        case 1:
            f->lo = get_tile_lo(ppu, f->tileid, f->obj_index);
            f->current_step = 2;
            break;
        case 2:
            f->hi = get_tile_hi(ppu, f->tileid, f->obj_index);
            f->current_step = 3;
            break;
        case 3:
            {
                if (queue_isempty(ppu->obj_fifo))
                    push_slice(ppu, ppu->obj_fifo, f->hi, f->lo, f->obj_index);
                else
                    merge_obj(ppu, f->hi, f->lo, f->obj_index);
                ppu->pop_pause = 0;
                ppu->has_pushed = 0;
                f->current_step = 0;
                return 0;
            }
    }

    return 2;
}

struct pixel select_pixel(struct ppu *ppu)
{
    struct pixel bg_p = queue_pop(ppu->bg_fifo);
    if (queue_isempty(ppu->obj_fifo))
        return bg_p;
    struct pixel obj_p = queue_pop(ppu->obj_fifo);
    if (!get_lcdc(ppu, 0))
        return obj_p;
    if (!get_lcdc(ppu, 1))
        return bg_p;
    if (obj_p.priority && bg_p.color != 0)
        return bg_p;
    if (obj_p.color == 0)
        return bg_p;
    return obj_p;
}

//STAT
void set_stat(struct ppu *ppu, int bit)
{
    *ppu->stat = *ppu->stat | (0x01 << bit);
}

int get_stat(struct ppu *ppu, int bit)
{
    return (*ppu->stat >> bit) & 0x01;
}

void clear_stat(struct ppu *ppu, int bit)
{
    *ppu->stat = *ppu->stat & ~(0x01 << bit);
}
