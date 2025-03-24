#include "emulator.h"
#include "cpu.h"
#include "render_table.h"

void emulator_init(Emulator* emulator) {
    audio_init(&emulator->audio_player);
    memory_init(&emulator->memory);
    rt_init(&emulator->render_table);
    cpu_init(&emulator->cpu);
}

void emulator_load_rom(Emulator* emulator, const char* rom_path) {
    FILE* rom_file = fopen(rom_path, "rb");

    if (rom_file == NULL) {
        printf("[ERROR] Unable to open the ROM you provided !\n");
        exit(EXIT_FAILURE);
    }

    uint8_t* rom_buffer;
    long file_len;

    fseek(rom_file, 0, SEEK_END);
    file_len = ftell(rom_file);
    rewind(rom_file);

    if (file_len > MAX_ROM_SIZE - 1) {
        printf("[ERROR] Your ROM is too big to fit into the memory");
        exit(EXIT_FAILURE);
    }

    rom_buffer = (uint8_t*)malloc(file_len * sizeof(uint8_t));
    fread(rom_buffer, file_len, 1, rom_file);
    fclose(rom_file);

    for (size_t i = 0; i < FONTSET_SIZE; i++) {
        memory_write(&emulator->memory, i, FONTSET[i]);
    }

    for (size_t i = 0; i < file_len; i++) {
        memory_write(&emulator->memory, i + PROCESSOR_INTERNAL_PROGRAM_COUNTER_START, rom_buffer[i]);
    }

    free(rom_buffer); // Don't need the buffer at this point.
}

bool emulator_is_pixel_on(Emulator* emulator, uint8_t x, uint8_t y) {
    return rt_is_pixel_on(&emulator->render_table, x, y);
}

void emulator_update_cpu_timers(Emulator* emulator) {
    cpu_update_timers(&emulator->cpu, &emulator->audio_player);
}

void emulator_execute_cpu_instruction(Emulator* emulator) {
    uint16_t opcode = cpu_fetch_next_instruction(&emulator->cpu, &emulator->memory);
    cpu_execute_instruction(&emulator->cpu, opcode, &emulator->render_table, &emulator->memory);
}

void emulator_deinit(Emulator* emulator) {
    audio_deinit(&emulator->audio_player);
    cpu_deinit(&emulator->cpu);
}
