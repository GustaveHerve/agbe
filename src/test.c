#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "cpu.h"
#include "emulation.h"
#include "ppu_utils.h"

int main(int argc, char **argv)
{
    if (argc != 2)
        return -1;

    if (SDL_Init(SDL_INIT_EVERYTHING))
        return EXIT_FAILURE;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    window = SDL_CreateWindow("AGBE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1120, 1008, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, 160, 144);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888,
            SDL_TEXTUREACCESS_STREAMING, 160, 144);

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, 160, 144, 32, SDL_PIXELFORMAT_RGB888);

    struct renderer *rend = malloc(sizeof(struct renderer));;
    rend->renderer = renderer;
    rend->window = window;
    rend->surface = surface;
    rend->texture = texture;

	struct cpu *cpu = malloc(sizeof(struct cpu));
	cpu_init(cpu, rend);

    main_loop(cpu, argv[1]);

    free_renderer(cpu->ppu->renderer);
    cpu_free(cpu);

    SDL_Quit();
	return 0;
}
