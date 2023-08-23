#include <stdlib.h>
#include "ppu_utils.h"

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
    res = res | ((slice << 7) & 0x80);
    res = res | ((slice << 5) & 0x40);
    res = res | ((slice << 3) & 0x20);
    res = res | ((slice << 1) & 0x10);
    res = res | ((slice >> 1) & 0x08);
    res = res | ((slice >> 3) & 0x04);
    res = res | ((slice >> 5) & 0x02);
    res = res | ((slice >> 7) & 0x01);
    return res;
}

