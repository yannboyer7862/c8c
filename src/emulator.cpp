#include "emulator.hpp"
#include <fstream>
#include <vector>
#include <iostream>

void Emulator::init_audio() {
    m_audio_player.init();
}

void Emulator::load_rom_from_file(const std::string& rom_path) {
    std::ifstream rom_file(rom_path, std::ios::binary);

    if (!rom_file.is_open())
        throw std::runtime_error("[ERROR] Unable to open the file !");

    std::size_t rom_buffer_size;
    std::vector<uint8_t> rom_buffer;

    rom_file.seekg(0, std::ios::end);
    rom_buffer_size = rom_file.tellg();
    rom_file.seekg(0, std::ios::beg);


    if (rom_buffer_size > MAX_ROM_SIZE - 1)
        throw std::runtime_error("[ERROR] Your ROM is too big to fit into the memory.");

    rom_buffer.reserve(rom_buffer_size);
    rom_file.read(reinterpret_cast<char*>(rom_buffer.data()), rom_buffer_size);
    rom_file.close();

    for (std::size_t i = 0; i < FONTSET_SIZE; i++) {
        m_memory.write(i, FONTSET[i]);
    }

    for (std::size_t i = 0; i < rom_buffer_size; i++) {
        m_memory.write(i + PROCESSOR_INTERNAL_PROGRAM_COUNTER_START, rom_buffer[i]);
    }

    std::cout << "[INFO] ROM loaded successfully loaded into memory !\n";
}

bool Emulator::is_pixel_on(uint8_t x, uint8_t y) {
    return m_render_table.is_pixel_on(x, y);
}

void Emulator::execute_cpu_instruction() {
    m_cpu.execute_instruction(m_render_table, m_memory);
}

void Emulator::update_cpu_timers() {
    m_cpu.update_timers(m_audio_player);
}
