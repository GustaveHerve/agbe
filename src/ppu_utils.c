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
