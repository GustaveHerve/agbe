#ifndef PPU_UTILS_H
#define PPU_UTILS_H

#include <stdlib.h>

struct pixel
{
    int color;
    int palette;
    int priority;
    int obj;
};

struct obj
{
    uint8_t *y;
    uint8_t *x;
    uint8_t *index;
    uint8_t *attributes;
};

struct pixel make_pixel(uint8_t hi, uint8_t lo, int i, uint8_t *attributes);


#endif
