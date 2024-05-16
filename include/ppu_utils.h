#ifndef PPU_UTILS_H
#define PPU_UTILS_H

#include <stdint.h>

struct ppu;
struct queue;

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

// in_window: read LX and LY and check if drawing in Window or BG
int in_window(struct ppu *ppu);

// in_object: reads the 10 OAM slots starting from obj_index and checks if we need to draw an object at current LX LY
// returns object index in obj_slots, -1 if no object
int in_object(struct ppu *ppu, int obj_index);

int push_pixel(struct queue *target, struct pixel p);

struct pixel pop_pixel(struct ppu *ppu, int obj);

struct pixel select_pixel(struct ppu *ppu);

int push_slice(struct ppu *ppu, struct queue *q, uint8_t hi, uint8_t lo, int obj_i);

//OBJ Merge version of push_slice in case it is not empty, overwrite transparent pixels OBJ FIFO
int merge_obj(struct ppu *ppu, uint8_t hi, uint8_t lo, int obj_i);

// STAT
void set_stat(struct ppu *ppu, int bit);
int get_stat(struct ppu *ppu, int bit);
void clear_stat(struct ppu *ppu, int bit);


void check_lyc(struct ppu *ppu);

#endif
