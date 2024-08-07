#ifndef PPU_UTILS_H
#define PPU_UTILS_H

#include <stdint.h>

struct ppu;
struct queue;

struct pixel
{
    int obj;
    uint8_t color;
    uint8_t palette;
    uint8_t priority;
};

struct obj
{
    uint8_t y;
    uint8_t x;
    uint8_t *oam_address;
    uint8_t done;
};

// on_window: read LX and LY and check if drawing in Window or BG
int on_window(struct ppu *ppu);

// on_object: reads the 10 OAM slots and checks if we need to draw an object at current LX LY
// returns object index in obj_slots, -1 if no object
int on_object(struct ppu *ppu, int *bottom_part);

int push_pixel(struct queue *target, struct pixel p);

struct pixel select_pixel(struct ppu *ppu);

int push_slice(struct ppu *ppu, struct queue *q, uint8_t hi, uint8_t lo, int obj_i);

uint8_t slice_xflip(uint8_t slice);

// OBJ Merge version of push_slice in case it is not empty, overwrite transparent pixels OBJ FIFO
int merge_obj(struct ppu *ppu, uint8_t hi, uint8_t lo, int obj_i);

void check_lyc(struct ppu *ppu, int line_153);

#endif
