#include "render_table.h"
#include <stdio.h>
#include <stdlib.h>

void rt_init(RenderTable* render_table) {
    for (size_t i = 0; i < RENDER_TABLE_SIZE; i++) {
        render_table->render_table[i] = OFF;
    }
}

bool rt_is_pixel_on(RenderTable* render_table, uint8_t x, uint8_t y) {
    if (x > CHIP8_SCREEN_WIDTH || y > CHIP8_SCREEN_HEIGHT) {
        printf("[ERROR] Invalid coordinates ! X -> %u Y -> %u", x, y);
        exit(EXIT_FAILURE);
    }

    return render_table->render_table[y * CHIP8_SCREEN_WIDTH + x] == ON;
}

void rt_change_pixel_state(RenderTable* render_table, uint8_t x, uint8_t y, PixelState new_state) {
    if (x > CHIP8_SCREEN_WIDTH || y > CHIP8_SCREEN_HEIGHT) {
        printf("[ERROR] Invalid coordinates ! X -> %u Y -> %u", x, y);
        exit(EXIT_FAILURE);
    }

    render_table->render_table[y * CHIP8_SCREEN_WIDTH + x] = new_state;
}

void rt_clear(RenderTable* render_table) {
    for (uint8_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
            rt_change_pixel_state(render_table, x, y, OFF);
        }
    }
}
