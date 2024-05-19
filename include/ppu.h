#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include "queue.h"
#include "rendering.h"

struct fetcher
{
    int obj_index;    //Used if PPU in OBJ mode
    int bottom_part;

    uint8_t attributes;
    uint8_t tileid; //Variables to Save state between dots
    uint8_t lo;
    uint8_t hi;

    uint8_t current_step; //0 = get_tile_id
                          //1 = get_tile_lo
                          //2 = get_tile_hi
                          //3 = push_pixels
    uint8_t tick;

    uint8_t lx_save;
};

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

    uint8_t *bgp;
    uint8_t *obp0;
    uint8_t *obp1;

    struct obj obj_slots[10];
    int8_t obj_count;

    struct queue *bg_fifo;
    struct queue *obj_fifo;

    struct fetcher *bg_fetcher;
    struct fetcher *obj_fetcher;

    uint8_t oam_locked;
    uint8_t vram_locked;

    uint8_t dma;
    uint8_t dma_acc;
    uint8_t dma_source;

    int line_dot_count; // Dot count for current scanline
    uint8_t mode1_153th;
    uint8_t first_tile;
    uint8_t current_mode;

    uint8_t win_mode;
    uint8_t win_ly; // Internal ly specific for the window which tells which allow us to "remember" which line of the window we were rendering
    uint8_t win_lx; // same for x
    uint8_t wy_trigger;

    uint8_t obj_mode;

    struct renderer *renderer;
};

void ppu_init(struct ppu *ppu, struct cpu *cpu, struct renderer *renderer);
void ppu_free(struct ppu *ppu);
void ppu_reset(struct ppu *ppu);

void ppu_tick_m(struct ppu *ppu);

int get_lcdc(struct ppu *ppu, int bit);

#endif
