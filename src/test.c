#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "cpu.h"
#include "emulation.h"

int main()
{

    if (SDL_Init(SDL_INIT_EVERYTHING))
        return EXIT_FAILURE;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
    SDL_RenderSetLogicalSize(renderer, 160, 144);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888,
            SDL_TEXTUREACCESS_STREAMING, 160, 144);

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, 160, 144, 32, SDL_PIXELFORMAT_RGB888);
    /*

    SDL_LockSurface(surface);
    Uint32 *pixels = (Uint32 *)surface->pixels;
    for (int i = 0; i < 160 * 144; i++)
        pixels[i] = SDL_MapRGB(surface->format, 224, 248, 208);
    SDL_UnlockSurface(surface);

    SDL_UpdateTexture(texture, NULL, surface->pixels, 160 * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_PumpEvents();
    SDL_Delay(5000);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    */

    struct renderer *rend = malloc(sizeof(struct renderer));;
    rend->renderer = renderer;
    rend->window = window;
    rend->surface = surface;
    rend->texture = texture;

	struct cpu *cpu = malloc(sizeof(struct cpu));
	cpu_init(cpu, rend);

    main_loop(cpu);

	cpu_free(cpu);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(surface);
    SDL_Quit();

    free(rend);

	return 0;
}
