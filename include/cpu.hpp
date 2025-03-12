#ifndef CPU_HPP
#define CPU_HPP

#include "render_table.hpp"
#include "memory.hpp"
#include "audio.hpp"

#include <cstdint>
#include <array>
#include <vector>


enum class KeyState {
    Pressed,
    NotPressed,
};

class CPU {
    public:
        CPU();

        void update_timers(AudioPlayer& audio_player);
        void execute_instruction(RenderTable& render_table, Memory& memory);
    private:
        uint16_t fetch_next_opcode(Memory& memory);

        static constexpr uint8_t REGISTERS_COUNT = 16;
        static constexpr uint8_t KEYS_COUNT = 16;

        std::array<uint8_t, REGISTERS_COUNT> m_registers;
        std::array<KeyState, KEYS_COUNT> m_keys;
        std::vector<uint16_t> m_stack;
        uint16_t m_index_register;

        uint8_t m_delay_timer;
        uint8_t m_sound_timer;

        uint16_t m_program_counter;
};

#endif
