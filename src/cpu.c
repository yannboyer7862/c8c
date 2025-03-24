#include "cpu.h"
#include "consts.h"
#include "render_table.h"
#include <time.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

void cpu_init(CPU* cpu) {
    for (size_t i = 0; i < REGISTERS_COUNT; i++) {
        cpu->registers[i] = 0;
    }

    for (size_t i = 0; i < KEYS_COUNT; i++) {
        cpu->keys[i] = NOT_PRESSED;
    }

    cpu->stack = NULL;
    cpu->index_register = 0;
    cpu->delay_timer = 0;
    cpu->sound_timer = 0;
    cpu->program_counter = PROCESSOR_INTERNAL_PROGRAM_COUNTER_START;
}

void cpu_update_timers(CPU* cpu, Audio* audio_player) {
    if (cpu->delay_timer > 0) cpu->delay_timer--;
    if (cpu->sound_timer > 0) {
        cpu->sound_timer--;

        if (cpu->sound_timer == 1)
            audio_play_beep_sound(audio_player);
    }
}

uint16_t cpu_fetch_next_instruction(CPU* cpu, Memory* memory) {
    uint16_t msb = memory_read(memory, cpu->program_counter);
    uint16_t lsb = memory_read(memory, cpu->program_counter + 1);

    return msb << 8 | lsb;
}

void cpu_execute_instruction(CPU* cpu, uint16_t opcode, RenderTable* render_table, Memory* memory) {
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t nn = opcode & 0x00FF;
    uint8_t n = opcode & 0x000F;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0:
                    // CLS
                    rt_clear(render_table);
                    cpu->program_counter += 2;
                    break;
                case 0x00EE:
                    // RET
                    cpu->program_counter = arrpop(cpu->stack);
                    cpu->program_counter += 2;
                    break;
                default:
                    printf("[ERROR] Unknown opcode -> 0x%04x\n", opcode);
                    exit(EXIT_FAILURE);
                    break;
            }
            break;
        case 0x1000:
            // JP addr
            cpu->program_counter = nnn;
            break;
        case 0x2000:
            // CALL addr
            arrpush(cpu->stack, cpu->program_counter);
            cpu->program_counter = nnn;
            break;
        case 0x3000:
            // SE Vx, byte
            if (cpu->registers[x] == nn) cpu->program_counter += 4;
            else cpu->program_counter += 2;
            break;
        case 0x4000:
            // SNE Vx, byte
            if (cpu->registers[x] != nn) cpu->program_counter += 4;
            else cpu->program_counter += 2;
            break;
        case 0x5000:
            // SE Vx, Vy
            if (cpu->registers[x] == cpu->registers[y]) cpu->program_counter += 4;
            else cpu->program_counter += 2;
            break;
        case 0x6000:
            // LD Vx, byte
            cpu->registers[x] = nn;
            cpu->program_counter += 2;
            break;
        case 0x7000:
            // ADD Vx, byte
            cpu->registers[x] += nn;
            cpu->program_counter += 2;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    // LD Vx, Vy
                    cpu->registers[x] = cpu->registers[y];
                    cpu->program_counter += 2;
                    break;
                case 0x0001:
                    // OR Vx, Vy
                    cpu->registers[x] |= cpu->registers[y];
                    cpu->program_counter += 2;
                    break;
                case 0x0002:
                    // AND Vx, Vy
                    cpu->registers[x] &= cpu->registers[y];
                    cpu->program_counter += 2;
                    break;
                case 0x0003:
                    // XOR Vx, Vy
                    cpu->registers[x] ^= cpu->registers[y];
                    cpu->program_counter += 2;
                    break;
                case 0x0004:
                    // ADD Vx, Vy
                    {
                        uint16_t r = cpu->registers[x] + cpu->registers[y];

                        cpu->registers[0xF] = r > 0xFF ? 1 : 0;

                        cpu->registers[x] = (uint8_t)(r & 0xFF);

                        cpu->program_counter += 2;
                    }
                    break;
                case 0x0005:
                    // SUB Vx, Vy
                    cpu->registers[0xF] = cpu->registers[x] > cpu->registers[y] ? 1 : 0;

                    cpu->registers[x] -= cpu->registers[y];

                    cpu->program_counter += 2;
                    break;
                case 0x0006:
                    // SHR Vx {, Vy}
                    cpu->registers[0xF] = cpu->registers[x] & 0x1;
                    cpu->registers[x] >>= 1;
                    cpu->program_counter += 2;
                    break;
                case 0x0007:
                    // SUBN Vx, Vy
                    cpu->registers[0xF] = cpu->registers[y] > cpu->registers[x];

                    cpu->registers[x] = cpu->registers[y] - cpu->registers[x];

                    cpu->program_counter += 2;

                    break;
                case 0x000E:
                    // SHL Vx {, Vy}
                    cpu->registers[0xF] = (cpu->registers[x] & 128) >> 7;
                    cpu->registers[x] <<= 1;
                    cpu->program_counter += 2;
                    break;
                default:
                    printf("[ERROR] Unknown opcode -> 0x%04x\n", opcode);
                    exit(EXIT_FAILURE);
                    break;
            }
            break;
        case 0x9000:
            // SNE Vx, Vy
            if (cpu->registers[x] != cpu->registers[y]) cpu->program_counter += 4;
            else cpu->program_counter += 2;
            break;
        case 0xA000:
            // LD I, addr
            cpu->index_register = nnn;
            cpu->program_counter += 2;
            break;
        case 0xB000:
            // JP V0, addr
            cpu->program_counter = nnn + cpu->registers[0x0];
            break;
        case 0xC000:
            // RND Vx, byte
            {
                srand(time(0));
                uint8_t random_number = rand() % 256; // Generate a number between 0 and 255
                cpu->registers[x] = random_number & nn;
                cpu->program_counter += 2;
            }
            break;
        case 0xD000:
            // DRW Vx, Vy, nibble
            {
                uint8_t origin_x = cpu->registers[x];
                uint8_t origin_y = cpu->registers[y];

                cpu->registers[0xF] = 0;

                for (uint8_t y_coord = 0; y_coord < n; y_coord++) {
                    uint8_t pixel = memory_read(memory, y_coord + cpu->index_register);
                    for (uint8_t x_coord = 0; x_coord < 8; x_coord++) {
                        if ((pixel & (0x80 >> x_coord)) != 0) {
                            uint8_t pixel_x = (origin_x + x_coord) % CHIP8_SCREEN_WIDTH;
                            uint8_t pixel_y = (origin_y + y_coord) % CHIP8_SCREEN_HEIGHT;

                            if (rt_is_pixel_on(render_table, pixel_x, pixel_y)) {
                                rt_change_pixel_state(render_table, pixel_x, pixel_y, OFF);
                                cpu->registers[0xF] = 1; // Collision.
                            } else {
                                rt_change_pixel_state(render_table, pixel_x, pixel_y, ON);
                            }
                        }
                    }
                }

                cpu->program_counter += 2;
            }
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E:
                    // SKP Vx
                    if (cpu->keys[cpu->registers[x]] == PRESSED) cpu->program_counter += 4;
                    else cpu->program_counter += 2;
                    break;
                case 0x00A1:
                    // SKNP Vx
                    if (cpu->keys[cpu->registers[x]] == NOT_PRESSED) cpu->program_counter += 4;
                    else cpu->program_counter += 2;
                    break;
                default:
                    printf("[ERROR] Unknown opcode -> 0x%04x\n", opcode);
                    exit(EXIT_FAILURE);
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    cpu->registers[x] = cpu->delay_timer;
                    cpu->program_counter += 2;
                    break;
                case 0x000A:
                    // LD Vx, K
                    // Not implemented yet.
                    printf("[ERROR] LD Vx, K instruction is not yet implemented !\n");
                    exit(EXIT_FAILURE);
                    break;
                case 0x0015:
                    // LD DT, Vx
                    cpu->delay_timer = cpu->registers[x];
                    cpu->program_counter += 2;
                    break;
                case 0x0018:
                    // LD ST, Vx
                    cpu->sound_timer = cpu->registers[x];
                    cpu->program_counter += 2;
                    break;
                case 0x001E:
                    // ADD I, Vx
                    cpu->index_register += cpu->registers[x];
                    cpu->program_counter += 2;
                    break;
                case 0x0029:
                    // LD F, Vx
                    cpu->index_register = 5 * cpu->registers[x];
                    cpu->program_counter += 2;
                    break;
                case 0x0033:
                    // LD B, Vx
                    {
                        uint8_t reg_val = cpu->registers[x];

                        memory_write(memory, cpu->index_register, reg_val / 100);
                        memory_write(memory, cpu->index_register + 1, (reg_val % 100) / 10);
                        memory_write(memory, cpu->index_register + 2, reg_val % 10);

                        cpu->program_counter += 2;
                    }
                    break;
                case 0x0055:
                    // LD [I], Vx
                    for (uint8_t i = 0; i < x + 1; i++) {
                        memory_write(memory, i + cpu->index_register, cpu->registers[i]);
                    }

                    cpu->program_counter += 2;
                    break;
                case 0x0065:
                    // LD Vx, [I]
                    for (uint8_t i = 0; i < x + 1; i++) {
                        cpu->registers[i] = memory_read(memory, i + cpu->index_register);
                    }

                    cpu->program_counter += 2;
                    break;
                default:
                    printf("[ERROR] Unknown opcode -> 0x%04x\n", opcode);
                    exit(EXIT_FAILURE);
                    break;
            }
            break;
        default:
            printf("[ERROR] Unknown opcode -> 0x%04x\n", opcode);
            exit(EXIT_FAILURE);
            break;
    }
}

void cpu_deinit(CPU* cpu) {
    arrfree(cpu->stack);
}
