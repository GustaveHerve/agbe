#ifndef PPU_H
#define PPU_H

#include <stdlib.h>
#include "queue.h"

struct fetcher
{
    uint8_t tileid; //Variables to Save state between dots
    uint8_t lo;
    uint8_t hi;

    uint8_t current_step; //0 = get_tile_id
                          //1 = get_tile_lo
                          //2 = get_tile_hi
                          //3 = push_pixels
    int obj_index;    //Used if PPU in OBJ mode
} typedef fetcher;


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

    fetcher *bg_fetcher;
    fetcher *obj_fetcher;

    uint8_t oam_locked;
    uint8_t vram_locked;
    uint8_t dma_oam_locked;

    int frame_dot_count;//Dot count for current frame
    int line_dot_count; //Dot count for current scanline
    uint8_t current_mode;

    uint8_t win_mode;
    uint8_t pop_pause;
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
struct pixel pop_pixel(struct ppu *ppu, int obj);
int merge_obj(struct ppu *ppu, uint8_t hi, uint8_t lo, int obj);

int get_lcdc(struct ppu *ppu, int bit);
int in_window(struct ppu *ppu);
int in_object(struct ppu *ppu, int obj_index);

//STAT
void set_stat(struct ppu *ppu, int bit);
void clear_stat(struct ppu *ppu, int bit);
int get_stat(struct ppu *ppu, int bit);
void stat_request(struct ppu *ppu, int bit);

//Fetcher functions
void fetcher_init(fetcher *f);
int bg_fetcher_step(struct ppu *ppu);
int obj_fetcher_step(struct ppu *ppu);

struct pixel select_pixel(struct ppu *ppu);
#endif
