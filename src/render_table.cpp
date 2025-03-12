#include "render_table.hpp"
#include <iostream>

RenderTable::RenderTable() {
    m_render_table.fill(PixelState::Off);
}

bool RenderTable::is_pixel_on(uint8_t x, uint8_t y) {
    if (x > CHIP8_SCREEN_WIDTH || y > CHIP8_SCREEN_HEIGHT) {
        std::cerr << "[ERROR] Invalid coordinates : X -> " << unsigned(x) << " Y -> " << unsigned (y) << "\n";
        std::exit(1);
    }

    return m_render_table[y * CHIP8_SCREEN_WIDTH + x] == PixelState::On;
}

void RenderTable::change_pixel_state(uint8_t x, uint8_t y, PixelState new_state) {
    if (x > CHIP8_SCREEN_WIDTH || y > CHIP8_SCREEN_HEIGHT) {
        std::cerr << "[ERROR] Invalid coordinates : X -> " << unsigned(x) << " Y -> " << unsigned (y) << "\n";
        std::exit(1);
    }

    m_render_table[y * CHIP8_SCREEN_WIDTH + x] = new_state;
}

void RenderTable::clear() {
    for (uint8_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
            change_pixel_state(x, y, PixelState::Off);
        }
    }
}
