#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "consts.h"
#include "emulator.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("[ERROR] No ROM provided !\n");
        printf("[INFO] Usage : ./c8c my_game.rom/my_game.ch8\n");
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("C8C - Chip8VM by Yann BOYER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("[ERROR] Unable to initialize the window !\n");
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (renderer == NULL) {
        printf("[ERROR] Unable to initialize the renderer !\n");
        return EXIT_FAILURE;
    }

    Emulator emulator;

    emulator_init(&emulator);

    const char* rom_path = argv[1];

    emulator_load_rom(&emulator, rom_path);

    uint8_t div_cycles = 0;

    bool is_running = true;

    while (is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    printf("[INFO] Exiting...\n");
                    is_running = false;
                    break;
                case SDL_KEYDOWN:
                    break;
                case SDL_KEYUP:
                    break;
                default: break;
            }
        }

        emulator_execute_cpu_instruction(&emulator);
        div_cycles++;

        // Draw
        for (uint8_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
            for (uint8_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
                SDL_Rect pixel = { x * PIXEL_SCALE_FACTOR, y * PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR };
                if (emulator_is_pixel_on(&emulator, x, y))
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &pixel);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        if (div_cycles == TIMER_CLOCK_DIVISION) {
            emulator_update_cpu_timers(&emulator);
            div_cycles = 0;
        }

        SDL_Delay(CPU_CLOCK_DELAY);
    }

    emulator_deinit(&emulator);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
