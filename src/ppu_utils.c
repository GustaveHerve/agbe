#include <stdlib.h>
#include "cpu.h"
#include "ppu_utils.h"
#include "queue.h"
#include "ppu.h"

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

struct pixel make_pixel(uint8_t hi, uint8_t lo, int i, uint8_t *attributes)
{
    struct pixel res;
    uint8_t hi_bit =  (hi >> (7-i)) & 0x01;
    uint8_t lo_bit = (lo >> (7-i)) & 0x01;
    res.color = (hi_bit << 1) | lo_bit;
    res.obj = 0;
    if (attributes != NULL)
    {
        res.palette = (*attributes >> 4) & 0x01;
        res.priority = (*attributes >> 7) & 0x01;
        res.obj = 1;
    }
    return res;
}

uint8_t slice_xflip(uint8_t slice)
{
    uint8_t res = 0x00;
    res |= ((slice << 7) & 0x80);
    res |= ((slice << 5) & 0x40);
    res |= ((slice << 3) & 0x20);
    res |= ((slice << 1) & 0x10);
    res |= ((slice >> 1) & 0x08);
    res |= ((slice >> 3) & 0x04);
    res |= ((slice >> 5) & 0x02);
    res |= ((slice >> 7) & 0x01);
    return res;
}

int in_window(struct ppu *ppu)
{
    return *ppu->ly >= *ppu->wy && ppu->lx >= *ppu->wx;
}

int in_object(struct ppu *ppu, int obj_index)
{
    // TODO handle 8x16 object mode
    for (int i = obj_index + 1; i < ppu->obj_count; ++i)
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

int push_pixel(struct queue *target, struct pixel p)
{
    queue_push(target, p);
    return 1;
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

int push_slice(struct ppu *ppu, struct queue *q, uint8_t hi, uint8_t lo, int obj_i)
{
    uint8_t* attributes = NULL;
    if (obj_i != -1)
        attributes = ppu->obj_slots[obj_i].oam_address + 3;
    for (int i = 0; i < 8; ++i)
    {
        struct pixel p = make_pixel(hi, lo, i, attributes);
        //TODO verify this
        if (!get_lcdc(ppu, 0))
            p.color = 0;
        push_pixel(q, p);
    }
    return 2;
}

int merge_obj(struct ppu *ppu, uint8_t hi, uint8_t lo, int obj_i)
{
    uint8_t *attributes = ppu->obj_slots[obj_i].oam_address + 3;
    struct queue_node *q = ppu->obj_fifo->front;
    int i = 0;
    while (q != NULL && i < 8)
    {
        //replace transparent pixels
        struct pixel p = make_pixel(hi, lo, i, attributes);
        if (q->data.color == 0)
            q->data = p;
        q = q->next;
        ++i;
    }
    return 2; //not sure
}

void check_lyc(struct ppu *ppu)
{
    if (*ppu->ly == *ppu->lyc)
    {
        set_stat(ppu, 2);
        if (ppu->line_dot_count == 0 && get_stat(ppu, 6)) //&& !get_if(ppu->cpu, 1))
            set_if(ppu->cpu, 1);
    }
    else
        clear_stat(ppu, 2);
}
