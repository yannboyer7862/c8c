#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <array>
#include <cstdint>

class Memory {
    public:
        Memory();

        void write(uint16_t addr, uint8_t data);
        uint8_t read(uint16_t addr);
    private:
        static constexpr uint16_t TOTAL_MEMORY_SIZE = 4096;

        std::array<uint8_t, TOTAL_MEMORY_SIZE> m_memory;
};

#endif
