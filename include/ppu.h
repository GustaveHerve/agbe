#ifndef PPU_H
#define PPU_H

#include <stdlib.h>

struct pixel
{
    int color;
    int palette;
    int priority;
};

struct obj
{
    uint8_t *x;
    uint8_t *y;
    uint8_t *index;
    uint8_t *attributes;
};

struct ppu
{
    struct cpu *cpu;
    uint8_t *oam;

    uint8_t *lcdc;
    uint8_t lx;
    uint8_t *ly;
    //int mode;
    struct obj *obj_slots;
};

#endif
