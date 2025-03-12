#include "cpu.hpp"
#include "consts.hpp"
#include <random>
#include <iostream>

CPU::CPU() {
    m_registers.fill(0x0);
    m_keys.fill(KeyState::NotPressed);
    m_stack.resize(0x0);
    m_index_register = 0x0;
    m_delay_timer = 0x0;
    m_sound_timer = 0x0;
    m_program_counter = PROCESSOR_INTERNAL_PROGRAM_COUNTER_START;
}


void CPU::update_timers(AudioPlayer& audio_player) {
    if (m_delay_timer > 0x0) m_delay_timer--;
    if (m_sound_timer > 0x0) {
        m_sound_timer--;

        if (m_sound_timer == 0x1)
            audio_player.play_beep_sound();
    }
}

uint16_t CPU::fetch_next_opcode(Memory& memory) {
    uint16_t msb = memory.read(m_program_counter);
    uint16_t lsb = memory.read(m_program_counter + 1);

    return msb << 8 | lsb;
}

void CPU::execute_instruction(RenderTable& render_table, Memory& memory) {
    uint16_t opcode = fetch_next_opcode(memory);

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
                    render_table.clear();
                    m_program_counter += 2;
                    break;
                case 0x00EE:
                    // RET
                    m_program_counter = m_stack.back();
                    m_stack.pop_back();
                    m_program_counter += 2;
                    break;
                default:
                    std::cerr << "[ERROR] Unknown instruction -> 0x" << std::hex << opcode << std::dec << "\n";
                    std::exit(1);
                    break;
            }
            break;
        case 0x1000:
            // JP addr
            m_program_counter = nnn;
            break;
        case 0x2000:
            // CALL addr
            m_stack.push_back(m_program_counter);
            m_program_counter = nnn;
            break;
        case 0x3000:
            // SE Vx, byte
            if (m_registers[x] == nn) m_program_counter += 4;
            else m_program_counter += 2;
            break;
        case 0x4000:
            // SNE Vx, byte
            if (m_registers[x] != nn) m_program_counter += 4;
            else m_program_counter += 2;
            break;
        case 0x5000:
            // SE Vx, Vy
            if (m_registers[x] == m_registers[y]) m_program_counter += 4;
            else m_program_counter += 2;
            break;
        case 0x6000:
            // LD Vx, byte
            m_registers[x] = nn;
            m_program_counter += 2;
            break;
        case 0x7000:
            // ADD Vx, byte
            m_registers[x] += nn;
            m_program_counter += 2;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    // LD Vx, Vy
                    m_registers[x] = m_registers[y];
                    m_program_counter += 2;
                    break;
                case 0x0001:
                    // OR Vx, Vy
                    m_registers[x] |= m_registers[y];
                    m_program_counter += 2;
                    break;
                case 0x0002:
                    // AND Vx, Vy
                    m_registers[x] &= m_registers[y];
                    m_program_counter += 2;
                    break;
                case 0x0003:
                    // XOR Vx, Vy
                    m_registers[x] ^= m_registers[y];
                    m_program_counter += 2;
                    break;
                case 0x0004:
                    // ADD Vx, Vy
                    {
                        uint16_t r = m_registers[x] + m_registers[y];

                        m_registers[0xF] = r > 0xFF ? 1 : 0;

                        m_registers[x] = static_cast<uint8_t>(r & 0xFF);

                        m_program_counter += 2;
                    }
                    break;
                case 0x0005:
                    // SUB Vx, Vy
                    m_registers[0xF] = m_registers[x] > m_registers[y] ? 1 : 0;
                    m_registers[x] -= m_registers[y];
                    m_program_counter += 2;
                    break;
                case 0x0006:
                    // SHR Vx {, Vy}
                    m_registers[0xF] = m_registers[x] & 0x1;
                    m_registers[x] >>= 1;
                    m_program_counter += 2;
                case 0x0007:
                    // SUBN Vx, Vy
                    m_registers[0xF] = m_registers[y] > m_registers[x] ? 1 : 0;
                    m_registers[x] = m_registers[y] - m_registers[x];
                    m_program_counter += 2;
                    break;
                case 0x000E:
                    // SHL Vx {, Vy}
                    m_registers[0xF] = (m_registers[x] & 128) >> 7;
                    m_registers[x] <<= 1;
                    m_program_counter += 2;
                    break;
                default:
                    std::cerr << "[ERROR] Unknown instruction -> 0x" << std::hex << opcode << std::dec << "\n";
                    std::exit(1);
                    break;
            }
            break;
        case 0x9000:
            // SNE Vx, Vy
            if (m_registers[x] != m_registers[y]) m_program_counter += 4;
            else m_program_counter += 2;
            break;
        case 0xA000:
            // LD I, addr
            m_index_register = nnn;
            m_program_counter += 2;
            break;
        case 0xB000:
            // JP V0, addr
            m_program_counter = nnn + m_registers[0x0];
            break;
        case 0xC000:
            // RND Vx, byte
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<unsigned int> distr(0x0, 0xFF);

                uint8_t random_number = distr(gen);

                m_registers[x] = random_number & nn;

                m_program_counter += 2;
            }
            break;
        case 0xD000:
            {
                uint8_t origin_x = m_registers[x];
                uint8_t origin_y = m_registers[y];

                m_registers[0xF] = 0;

                for (uint8_t y_coord = 0; y_coord < n; y_coord++) {
                    uint8_t pixel = memory.read(m_index_register + y_coord);
                    for (uint8_t x_coord = 0; x_coord < 8; x_coord++) {
                        if ((pixel & (0x80 >> x_coord)) != 0) {
                            uint8_t pixel_x = (origin_x + x_coord) % CHIP8_SCREEN_WIDTH;
                            uint8_t pixel_y = (origin_y + y_coord) % CHIP8_SCREEN_HEIGHT;

                            if (render_table.is_pixel_on(pixel_x, pixel_y)) {
                                render_table.change_pixel_state(pixel_x, pixel_y, PixelState::Off);
                                m_registers[0xF] = 1; // Collision.
                            } else {
                                render_table.change_pixel_state(pixel_x, pixel_y, PixelState::On);
                            }
                        }
                    }
                }

                m_program_counter += 2;
            }
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E:
                    // SKP Vx
                    if (m_keys[m_registers[x]] == KeyState::Pressed) m_program_counter += 4;
                    else m_program_counter += 2;
                    break;
                case 0x00A1:
                    if (m_keys[m_registers[x]] == KeyState::NotPressed) m_program_counter += 4;
                    else m_program_counter += 2;
                    break;
                default:
                    std::cerr << "[ERROR] Unknown instruction -> 0x" << std::hex << opcode << std::dec << "\n";
                    std::exit(1);
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    // LD Vx, DT
                    m_registers[x] = m_delay_timer;
                    m_program_counter += 2;
                    break;
                case 0x000A:
                    // LD Vx, K
                    std::cerr << "[ERROR] This instruction isn't implemented for now -> LD Vx, K\n";
                    std::exit(1);
                    break;
                case 0x0015:
                    // LD DT, Vx
                    m_delay_timer = m_registers[x];
                    m_program_counter += 2;
                    break;
                case 0x0018:
                    // LD ST, Vx
                    m_sound_timer = m_registers[x];
                    m_program_counter += 2;
                    break;
                case 0x001E:
                    // ADD I, Vx
                    m_index_register += m_registers[x];
                    m_program_counter += 2;
                    break;
                case 0x0029:
                    // LD F, Vx
                    m_index_register = m_registers[x] * 5;
                    m_program_counter += 2;
                    break;
                case 0x0033:
                    // LD B, Vx
                    {
                        uint8_t reg_val = m_registers[x];

                        memory.write(m_index_register, reg_val / 100); // Hundreds.
                        memory.write(m_index_register + 1, (reg_val % 100) / 10); // Tens.
                        memory.write(m_index_register + 2, reg_val % 10); // Units.

                        m_program_counter += 2;
                    }
                    break;
                case 0x0055:
                    // LD [I], Vx
                    for (uint8_t i = 0; i < x + 1; i++) {
                        memory.write(i + m_index_register, m_registers[i]);
                    }

                    m_program_counter += 2;
                    break;
                case 0x0065:
                    // LD Vx, [I]
                    for (uint8_t i = 0; i < x + 1; i++) {
                        m_registers[i] = memory.read(i + m_index_register);
                    }

                    m_program_counter += 2;
                    break;
                default:
                    std::cerr << "[ERROR] Unknown instruction -> 0x" << std::hex << opcode << std::dec << "\n";
                    std::exit(1);
                    break;
            }
            break;
        default:
            std::cerr << "[ERROR] Unknown instruction -> 0x" << std::hex << opcode << std::dec << "\n";
            std::exit(1);
            break;
    }
}
