#ifndef RENDER_TABLE_HPP
#define RENDER_TABLE_HPP

#include <cstdint>
#include <array>
#include "consts.hpp"

enum class PixelState {
    On,
    Off,
};

class RenderTable {
    public:
        RenderTable();

        bool is_pixel_on(uint8_t x, uint8_t y);
        void change_pixel_state(uint8_t x, uint8_t y, PixelState new_state);
        void clear();
    private:
        static constexpr uint16_t RENDER_TABLE_SIZE = CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT;

        std::array<PixelState, RENDER_TABLE_SIZE> m_render_table;
};

#endif
