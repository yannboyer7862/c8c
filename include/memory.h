#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define TOTAL_MEMORY_SIZE 4096

typedef struct {
    uint8_t memory[TOTAL_MEMORY_SIZE];
} Memory;

void memory_init(Memory* memory);
void memory_write(Memory* memory, uint16_t addr, uint8_t value);
uint8_t memory_read(Memory* memory, uint16_t addr);

#endif
