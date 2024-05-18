#include <SDL2/SDL.h>
#include "cpu.h"
#include "ppu.h"
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
        //TODO cleaner handler...
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    {
                        switch (event.key.keysym.sym)
                        {
                            case SDLK_RIGHT:
                                cpu->joyp_d &= ~(0x01);
                                break;
                            case SDLK_LEFT:
                                cpu->joyp_d &= ~(0x02);
                                break;
                            case SDLK_UP:
                                cpu->joyp_d &= ~(0x04);
                                break;
                            case SDLK_DOWN:
                                cpu->joyp_d &= ~(0x08);
                                break;

                            case SDLK_z:
                                cpu->joyp_a &= ~(0x01);
                                break;
                            case SDLK_x:
                                cpu->joyp_a &= ~(0x02);
                                break;
                            case SDLK_SPACE:
                                cpu->joyp_a &= ~(0x04);
                                break;
                            case SDLK_RETURN:
                                cpu->joyp_a &= ~(0x08);
                                break;

                        }
                        break;
                    }
                case SDL_KEYUP:
                    {
                        switch (event.key.keysym.sym)
                        {
                            case SDLK_RIGHT:
                                cpu->joyp_d |= 0x01;
                                break;
                            case SDLK_LEFT:
                                cpu->joyp_d |= 0x02;
                                break;
                            case SDLK_UP:
                                cpu->joyp_d |= 0x04;
                                break;
                            case SDLK_DOWN:
                                cpu->joyp_d |= 0x08;
                                break;

                            case SDLK_z:
                                cpu->joyp_a |= 0x01;
                                break;
                            case SDLK_x:
                                cpu->joyp_a |= 0x02;
                                break;
                            case SDLK_SPACE:
                                cpu->joyp_a |= 0x04;
                                break;
                            case SDLK_RETURN:
                                cpu->joyp_a |= 0x08;
                                break;
                        }
                    }
                    break;
                case SDL_QUIT:
                    cpu->running = 0;
                    return;
            }

        }
        SDL_UpdateTexture(rend->texture, NULL, pixels, 160 * sizeof(Uint32));
        SDL_RenderClear(rend->renderer);
        SDL_RenderCopy(rend->renderer, rend->texture, NULL, NULL);
        SDL_RenderPresent(rend->renderer);
        //SDL_Delay(16);
    }
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
    for (int i = 0; i < total; ++i)
        pixels[i] = sdlPixel;

    SDL_UnlockSurface(rend->surface);
    SDL_UpdateTexture(rend->texture, NULL, pixels, 160 * sizeof(Uint32));
    SDL_RenderClear(rend->renderer);
    SDL_RenderCopy(rend->renderer, rend->texture, NULL, NULL);
    SDL_RenderPresent(rend->renderer);
    //SDL_PumpEvents();
}

void free_renderer(struct renderer *rend)
{
    SDL_DestroyWindow(rend->window);
    SDL_DestroyRenderer(rend->renderer);
    SDL_DestroyTexture(rend->texture);
    SDL_FreeSurface(rend->surface);
    free(rend);
}
