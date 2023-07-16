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

    queue *bg_fifo;
    queue *obj_fifo;

    int oam_locked;
    int vram_locked;
};

void ppu_init(struct ppu *ppu, struct cpu *cpu);
void ppu_tick(struct ppu *ppu);
int oam_scan(struct ppu *ppu);
int drawing_pixels(struct ppu *ppu);
int get_lcdc(struct ppu *ppu, int bit);
int in_window(struct ppu *ppu);

#endif
