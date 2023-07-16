#ifndef PPU_UTILS_H
#define PPU_UTILS_H

#include <stdlib.h>

struct pixel
{
    int color;
    int palette;
    int priority;
};

struct obj
{
    uint8_t *y;
    uint8_t *x;
    uint8_t *index;
    uint8_t *attributes;
};


#endif
