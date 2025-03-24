#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "audio.h"
#include "memory.h"
#include "render_table.h"

#define REGISTERS_COUNT 16
#define KEYS_COUNT 16

typedef enum {
    PRESSED,
    NOT_PRESSED,
} KeyState;

typedef struct {
    uint8_t registers[REGISTERS_COUNT];
    KeyState keys[KEYS_COUNT];
    uint16_t* stack;
    uint16_t index_register;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t program_counter;
} CPU;

void cpu_init(CPU* cpu);
void cpu_update_timers(CPU* cpu, Audio* audio_player);
uint16_t cpu_fetch_next_instruction(CPU* cpu, Memory* memory);
void cpu_execute_instruction(CPU* cpu, uint16_t opcode, RenderTable* render_table, Memory* memory);
void cpu_deinit(CPU* cpu);

#endif
