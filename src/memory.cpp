#include "memory.hpp"
#include <iostream>

Memory::Memory() {
    m_memory.fill(0x0);
}

void Memory::write(uint16_t addr, uint8_t data) {
    if (addr > TOTAL_MEMORY_SIZE - 1) {
        std::cerr << "[ERROR] Memory : WRITE command is out of range !\n";
        std::exit(1);
    }

    m_memory[addr] = data;
}

uint8_t Memory::read(uint16_t addr) {
    if (addr > TOTAL_MEMORY_SIZE - 1) {
        std::cerr << "[ERROR] Memory : Read command is out of range !\n";
        std::exit(1);
    }

    return m_memory[addr];
}
