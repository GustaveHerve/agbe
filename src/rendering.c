#include "SDL_events.h"
#include "cpu.h"
#include "emulation.h"
#include "ppu.h"
#include "ppu_utils.h"

#define WIDTH 160
#define HEIGHT 144
#define SCREEN_RESOLUTION (WIDTH * HEIGHT)

static Uint32 frame_buffer[SCREEN_RESOLUTION] = {0};

struct color
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

static struct color color_palette[4] = {{224, 248, 208}, {136, 192, 112}, {52, 104, 86}, {8, 24, 32}};

void draw_pixel(struct cpu *cpu, struct pixel p)
{
    struct renderer *rend = cpu->ppu->renderer;

    uint8_t *c_regist = p.obj > -1 ? (p.palette ? cpu->ppu->obp1 : cpu->ppu->obp0) : cpu->ppu->bgp;

    unsigned int color_index = (*c_regist >> (p.color * 2)) & 0x03;
    struct color *color = color_palette + color_index;

    Uint32 pixel = SDL_MapRGB(rend->format, color->r, color->g, color->b);
    frame_buffer[*cpu->ppu->ly * WIDTH + (cpu->ppu->lx - 8)] = pixel;

    // Render a frame and handle inputs
    if (*cpu->ppu->ly == HEIGHT - 1 && cpu->ppu->lx == WIDTH + 7)
    {
        // TODO: replace by a vblank callback like function that will run the SDL event handler loop and
        // synchronize the emulation speed according to elapsed cycle count
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

                case SDLK_x:
                    cpu->joyp_a &= ~(0x01);
                    break;
                case SDLK_z:
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

                case SDLK_x:
                    cpu->joyp_a |= 0x01;
                    break;
                case SDLK_z:
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

        SDL_UpdateTexture(rend->texture, NULL, frame_buffer, WIDTH * sizeof(Uint32));
        SDL_RenderClear(rend->renderer);
        SDL_RenderCopy(rend->renderer, rend->texture, NULL, NULL);
        SDL_RenderPresent(rend->renderer);

        synchronize(cpu);
    }
}

void lcd_off(struct cpu *cpu)
{
    struct renderer *rend = cpu->ppu->renderer;
    Uint32 color = SDL_MapRGB(rend->format, 229, 245, 218);

    for (size_t i = 0; i < SCREEN_RESOLUTION; ++i)
    {
        frame_buffer[i] = color;
    }

    SDL_UpdateTexture(rend->texture, NULL, frame_buffer, WIDTH * sizeof(Uint32));
    SDL_RenderClear(rend->renderer);
    SDL_RenderCopy(rend->renderer, rend->texture, NULL, NULL);
    SDL_RenderPresent(rend->renderer);
}

void free_renderer(struct renderer *rend)
{
    SDL_DestroyWindow(rend->window);
    SDL_DestroyRenderer(rend->renderer);
    SDL_DestroyTexture(rend->texture);
    free(rend);
}
