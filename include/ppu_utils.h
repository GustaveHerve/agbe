#ifndef PPU_UTILS_H
#define PPU_UTILS_H

#include <stdlib.h>
#include <stdint.h>

struct pixel
{
    uint8_t color;
    uint8_t palette;
    uint8_t priority;
    uint8_t obj;
};

struct obj
{
    uint8_t y;
    uint8_t x;
    uint8_t *oam_address;
};

struct pixel make_pixel(uint8_t hi, uint8_t lo, int i, uint8_t *attributes);
uint8_t slice_xflip(uint8_t slice);

#endif
