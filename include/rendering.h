#ifndef RENDERING_H
#define RENDERING_H

#include <SDL2/SDL.h>

struct cpu;
struct pixel;

struct renderer
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    SDL_Window *window;
};

void draw_pixel(struct cpu *cpu, struct pixel p);

#endif
