#include <SDL2/SDL.h>
#include "cpu.h"
#include "ppu_utils.h"

void draw_pixel(struct cpu *cpu, struct pixel p)
{
    struct renderer *rend = cpu->ppu->renderer;
    Uint32 *pixels = (Uint32 *)rend->surface->pixels;
    Uint32 sdlPixel;
    uint8_t *c_regist;
    if (p.obj)
    {
        if (p.palette)
            c_regist = cpu->ppu->obp1;
        else
            c_regist = cpu->ppu->obp0;
    }
    else
        c_regist = cpu->ppu->bgp;

    int color = (*c_regist >> (p.color * 2)) & 0x03;
    Uint8 r,g,b;
    switch (color)
    {
        case 0:
            r = 224;
            g = 248;
            b = 208;
            break;
        case 1:
            r = 136;
            g = 192;
            b = 112;
            break;
        case 2:
            r = 52;
            g = 104;
            b = 86;
            break;
        case 3:
            r = 8;
            g = 24;
            b = 32;
            break;
    }
    SDL_LockSurface(rend->surface);
    sdlPixel = SDL_MapRGB(rend->surface->format, r, g, b);
    pixels[*cpu->ppu->ly * 160 + (cpu->ppu->lx - 8)] = sdlPixel;

    if (*cpu->ppu->ly == 143 && cpu->ppu->lx == 167)
    {
        SDL_UnlockSurface(rend->surface);
        SDL_UpdateTexture(rend->texture, NULL, pixels, 160 * sizeof(Uint32));
        SDL_RenderClear(rend->renderer);
        SDL_RenderCopy(rend->renderer, rend->texture, NULL, NULL);
        SDL_RenderPresent(rend->renderer);
        SDL_PumpEvents();
        SDL_LockSurface(rend->surface);
    }
}
