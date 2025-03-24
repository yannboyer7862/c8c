#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

void memory_init(Memory* memory) {
    for (size_t i = 0; i < TOTAL_MEMORY_SIZE; i++) {
        memory->memory[i] = 0;
    }
}

void memory_write(Memory* memory, uint16_t addr, uint8_t value) {
    if (addr > TOTAL_MEMORY_SIZE - 1) {
        printf("[ERROR] Memory : WRITE command is out of range, ADDR -> %hu!\n", addr);
        exit(EXIT_FAILURE);
    }

    memory->memory[addr] = value;
}

uint8_t memory_read(Memory* memory, uint16_t addr) {
    if (addr > TOTAL_MEMORY_SIZE - 1) {
        printf("[ERROR] Memory : WRITE command is out of range, ADDR -> %hu!\n", addr);
        exit(EXIT_FAILURE);
    }

    return memory->memory[addr];
}
