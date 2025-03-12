#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <exception>
#include <SDL2/SDL.h>
#include "consts.hpp"
#include "emulator.hpp"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "[ERROR] Usage : ./c8c my_game.rom/my_game.ch8\n";
        std::cerr << "[ERROR] No ROM provided !\n";
        return EXIT_FAILURE;
    }

    std::string rom_path = std::string(argv[1]);

    SDL_Window* window = SDL_CreateWindow("C8C - Chip8VM by Yann BOYER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "[ERROR] Unable to initialize the window !\n";
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (renderer == nullptr) {
        std::cerr << "[ERROR] Unable to initialize the renderer !\n";
        return EXIT_FAILURE;
    }

    Emulator chip8_emulator;

    try {
        chip8_emulator.init_audio();
        chip8_emulator.load_rom_from_file(rom_path);
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    bool is_running = true;

    uint8_t div_cycles = 0;

    while (is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch ((event.type)) {
                case SDL_QUIT:
                    is_running = false;
                    std::cerr << "[INFO] Exiting...\n";
                    break;
                case SDL_KEYDOWN:
                    break;
                case SDL_KEYUP:
                    break;
                default: break;
            }
        }

        chip8_emulator.execute_cpu_instruction();
        div_cycles++;

        // Draw.
        for (uint8_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++)  {
            for (uint8_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
                SDL_Rect pixel = SDL_Rect { x * PIXEL_SCALE_FACTOR, y * PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR };
                if (chip8_emulator.is_pixel_on(x, y))
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &pixel);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        if (div_cycles == TIMER_CLOCK_DIVISION) {
            chip8_emulator.update_cpu_timers();
            div_cycles = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(CPU_CLOCK_DELAY));
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
