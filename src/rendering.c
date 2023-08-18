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
    SDL_UnlockSurface(rend->surface);

    if (*cpu->ppu->ly == 143 && cpu->ppu->lx == 167)
    {
        SDL_UpdateTexture(rend->texture, NULL, pixels, 160 * sizeof(Uint32));
        SDL_RenderClear(rend->renderer);
        SDL_RenderCopy(rend->renderer, rend->texture, NULL, NULL);
        SDL_RenderPresent(rend->renderer);
        SDL_PumpEvents();
        //SDL_Delay(17);
    }
}

void init_vram(struct ppu *ppu)
{
    for (int i = 0x8000; i < 0x9800; i++)
        ppu->cpu->membus[i] = 0;

    ppu->cpu->membus[0x9010] = 0xFF;
    ppu->cpu->membus[0x9011] = 0xFF;
    ppu->cpu->membus[0x9012] = 0xFF;
    ppu->cpu->membus[0x9013] = 0xFF;
    ppu->cpu->membus[0x9014] = 0xFF;
    ppu->cpu->membus[0x9015] = 0xFF;
    ppu->cpu->membus[0x9016] = 0xFF;
    ppu->cpu->membus[0x9017] = 0xFF;
    ppu->cpu->membus[0x9018] = 0xFF;
    ppu->cpu->membus[0x9019] = 0xFF;
    ppu->cpu->membus[0x901A] = 0xFF;
    ppu->cpu->membus[0x901B] = 0xFF;
    ppu->cpu->membus[0x901C] = 0xFF;
    ppu->cpu->membus[0x901D] = 0xFF;
    ppu->cpu->membus[0x901E] = 0xFF;
    ppu->cpu->membus[0x901F] = 0xFF;
    //ppu->cpu->membus[0x9000] = 0xFF;
    //ppu->cpu->membus[0x9001] = 0xFF;

    ppu->cpu->membus[0x9800] = 0x01;
    ppu->cpu->membus[0x99E0] = 0x01;
    ppu->cpu->membus[0x9A30] = 0x01;
}

void lcd_off(struct cpu *cpu)
{
    struct renderer *rend = cpu->ppu->renderer;
    Uint32 *pixels = (Uint32 *)rend->surface->pixels;
    Uint32 sdlPixel;
    SDL_LockSurface(rend->surface);
    sdlPixel = SDL_MapRGB(rend->surface->format, 224, 248, 208);
    int total = 160 * 144;
    //memset(pixels, sdlPixel, total);
    for (int i = 0; i < total; i++)
        pixels[i] = sdlPixel;

    SDL_UnlockSurface(rend->surface);
    SDL_UpdateTexture(rend->texture, NULL, pixels, 160 * sizeof(Uint32));
    SDL_RenderClear(rend->renderer);
    SDL_RenderCopy(rend->renderer, rend->texture, NULL, NULL);
    SDL_RenderPresent(rend->renderer);
    SDL_PumpEvents();
}
