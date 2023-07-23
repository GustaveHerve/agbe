#ifndef PPU_H
#define PPU_H

#include <stdlib.h>
#include "queue.h"

struct ppu
{
    struct cpu *cpu;
    uint8_t *oam;

    uint8_t *lcdc;
    uint8_t lx;
    uint8_t *ly;
    uint8_t *lyc;

    uint8_t *scy;
    uint8_t *scx;
    uint8_t *wy;
    uint8_t *wx;
    uint8_t *stat;

    struct obj obj_slots[10];
    int8_t obj_count;

    queue *bg_fifo;
    queue *obj_fifo;

    uint8_t oam_locked;
    uint8_t vram_locked;
    uint8_t dma_oam_locked;

    int frame_dot_count;//Dot count for current frame
    int line_dot_count; //Dot count for current scanline
    int mode_dot;       //Dot count for current mode
    uint8_t current_mode;

    uint8_t win_mode;
};

struct fetcher
{
    queue *target;
    uint8_t obj_fetcher;

    uint8_t tileid; //Variables to Save state between dots
    uint8_t lo;
    uint8_t hi;

    uint8_t current_step; //0 = get_tile_id
                          //1 = get_tile_lo
                          //2 = get_tile_hi
                          //3 = push_pixels
};

void ppu_init(struct ppu *ppu, struct cpu *cpu);
void ppu_free(struct ppu *ppu);

void ppu_tick_m(struct ppu *ppu);

//Mode 2
int oam_scan(struct ppu *ppu);
int oam_scan_m(struct ppu *ppu);

//Mode 3 functions
uint8_t get_tileid(struct ppu *ppu, int obj_index);
uint8_t get_tile_lo(struct ppu *ppu, uint8_t tileid, int obj_index);
uint8_t get_tile_hi(struct ppu *ppu, uint8_t tileid, int obj_index);
int push_pixel(queue *target, struct pixel p);
int push_slice(struct ppu *ppu, queue *q, uint8_t hi, uint8_t lo, int obj_i);

int hblank(struct ppu *ppu, int prev_time);
int vblank(struct ppu *ppu);

int get_lcdc(struct ppu *ppu, int bit);
int in_window(struct ppu *ppu);
int in_object(struct ppu *ppu);

//Fetcher functions
int fetcher_step(struct fetcher *f, struct ppu *ppu, int obj_index);

#endif
