#include "cpu.h"
#include <iostream>

CPU::CPU() {
    for(int i = 0; i < 16; i++) registers[i] = 0;
    registers[PC] = 0x00000000; // Começa no início da memória
    registers[SP] = 0x01000000; // Começa no topo da Stack (16MB)
}

bool CPU::setTecladoVirtual(int tecla, bool estado) {
    if (tecla < 0 || tecla >= 16) return false;
    teclado_virtual[tecla] = estado;
    return true;
}

void CPU::setRegister(int reg, uint32_t value) {
    if (reg == 0) return;
    registers[reg] = value;
}

uint32_t CPU::getRegister(int reg) const {
    if (reg == 0) return 0;
    return registers[reg];
}

void CPU::step(Memory& memory) {
    // 1. Busca (Fetch)
    uint32_t pc_current = registers[15];
    uint32_t instruction = memory.read32(pc_current);
    
    // Avança o PC para a próxima instrução (4 bytes adiante)
    registers[15] += 4;

    // 2. Decodificação (Decode)
    uint8_t opcode = (instruction >> 26) & 0x3F;
    uint8_t rx     = (instruction >> 21) & 0x1F;
    uint8_t ry     = (instruction >> 16) & 0x1F;
    uint8_t rz     = (instruction >> 11) & 0x1F;
    uint16_t imm   = instruction & 0xFFFF;

    // 3. Execução (Execute)
    switch (opcode) {
        // =====================================================================
        // INSTRUÇÕES TIPO R (Aritméticas e Lógicas por Registradores)
        // =====================================================================
        case 0x00: { // ADD rx, ry, rz -> rx = ry + rz
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            setRegister(rx, valY + valZ);
            break;
        }

        case 0x01: { // SUB rx, ry, rz -> rx = ry - rz
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            setRegister(rx, valY - valZ);
            break;
        }

        case 0x02: { // MUL rx, ry, rz -> rx = ry * rz
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            setRegister(rx, valY * valZ);
            break;
        }

        case 0x03: { // DIV rx, ry, rz -> rx = ry / rz
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            if (valZ != 0) {
                setRegister(rx, valY / valZ);
            } else {
                // Tratamento opcional para divisão por zero
                setRegister(rx, 0); 
            }
            break;
        }

        case 0x04: { // MOD rx, ry, rz -> rx = ry % rz
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            if (valZ != 0) {
                setRegister(rx, valY % valZ);
            } else {
                setRegister(rx, 0);
            }
            break;
        }

        case 0x05: { // AND rx, ry, rz -> rx = ry & rz
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            setRegister(rx, valY & valZ);
            break;
        }

        case 0x06: { // OR rx, ry, rz -> rx = ry | rz
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            setRegister(rx, valY | valZ);
            break;
        }

        case 0x07: { // XOR rx, ry, rz -> rx = ry ^ rz
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            setRegister(rx, valY ^ valZ);
            break;
        }

        case 0x08: { // SHL rx, ry, rz -> rx = ry << (rz & 0x1F)
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            setRegister(rx, valY << (valZ & 0x1F));
            break;
        }

        case 0x09: { // SHR rx, ry, rz -> rx = ry >> (rz & 0x1F)
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            setRegister(rx, valY >> (valZ & 0x1F));
            break;
        }

        case 0x0A: { // ROL rx, ry, rz (Rotate Left)
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            uint32_t shift = valZ & 0x1F;
            setRegister(rx, (valY << shift) | (valY >> (32 - shift)));
            break;
        }

        case 0x0B: { // ROR rx, ry, rz (Rotate Right)
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            uint32_t shift = valZ & 0x1F;
            setRegister(rx, (valY >> shift) | (valY << (32 - shift)));
            break;
        }

        // =================================================================
        // INSTRUÇÕES TIPO I (Operações com Imediato de 18 bits: bits 17..0)
        // =================================================================
        case 0x0C: { // ADDI rx, ry, imm18
            uint32_t valY = getRegister(ry);
            int32_t sign_extended_imm = (int16_t)imm; 
            setRegister(rx, valY + sign_extended_imm);
            break;
        }

        case 0x0D: { // MOVL rx, ry, imm18
            uint32_t imm18 = instruction & 0x3FFFF;
            int32_t signed_imm = (imm18 & 0x20000) ? (imm18 | 0xFFFC0000) : imm18;
            if (rx != 0) registers[rx] = registers[ry] - signed_imm;
            break;
        }

        case 0x0E: { // MOVH rx, ry, imm18
            uint32_t imm18 = instruction & 0x3FFFF;
            if (rx != 0) registers[rx] = registers[ry] << imm18;
            break;
        }

        case 0x0F: { // LOAD rx, ry, imm18
            uint32_t target_addr = getRegister(ry) + (int16_t)imm;
            uint32_t val = memory.read32(target_addr);
            setRegister(rx, val);
            break;
        }

        case 0x10: { // STORE rx, ry, imm18 (rx = Memory[ry + imm18])
            uint32_t imm18 = instruction & 0x3FFFF;
            int32_t signed_imm = (imm18 & 0x20000) ? (imm18 | 0xFFFC0000) : imm18;
            uint32_t target_addr = registers[ry] + signed_imm;
            if (rx != 0) registers[rx] = memory.read32(target_addr);
            break;
        }

        case 0x11: { // BEQ rx, ry, imm18 (Memory[ry + imm18] = rx)
            uint32_t imm18 = instruction & 0x3FFFF;
            int32_t signed_imm = (imm18 & 0x20000) ? (imm18 | 0xFFFC0000) : imm18;
            uint32_t target_addr = registers[ry] + signed_imm;
            memory.write32(target_addr, registers[rx]);
            break;
        }

        case 0x12: { // BNE rx, ry, imm18 (Branch if Equal)
            uint32_t imm18 = instruction & 0x3FFFF;
            int32_t signed_imm = (imm18 & 0x20000) ? (imm18 | 0xFFFC0000) : imm18;
            if (registers[rx] == registers[ry]) {
                registers[15] += (signed_imm * 4) - 4;
            }
            break;
        }

        case 0x13: { // BLT rx, ry, imm18 (Branch if Not Equal)
            uint32_t imm18 = instruction & 0x3FFFF;
            int32_t signed_imm = (imm18 & 0x20000) ? (imm18 | 0xFFFC0000) : imm18;
            if (registers[rx] != registers[ry]) {
                registers[15] += (signed_imm * 4) - 4;
            }
            break;
        }

        case 0x14: { // BGT rx, ry, imm18 (Branch if Less Than - Com sinal)
            uint32_t imm18 = instruction & 0x3FFFF;
            int32_t signed_imm = (imm18 & 0x20000) ? (imm18 | 0xFFFC0000) : imm18;
            if (static_cast<int32_t>(registers[rx]) < static_cast<int32_t>(registers[ry])) {
                registers[15] += (signed_imm * 4) - 4;
            }
            break;
        }

        case 0x15: { // BLE rx, ry, imm18 (Branch if Less Than - Com sinal)
            uint32_t imm18 = instruction & 0x3FFFF;
            int32_t signed_imm = (imm18 & 0x20000) ? (imm18 | 0xFFFC0000) : imm18;
            if (static_cast<int32_t>(registers[rx]) < static_cast<int32_t>(registers[ry])) {
                registers[15] += (signed_imm * 4) - 4;
            }
            break;
        }

        case 0x16: { // BGE rx, ry, imm18 (Branch if Less Than - Com sinal)
            uint32_t imm18 = instruction & 0x3FFFF;
            int32_t signed_imm = (imm18 & 0x20000) ? (imm18 | 0xFFFC0000) : imm18;
            if (static_cast<int32_t>(registers[rx]) < static_cast<int32_t>(registers[ry])) {
                registers[15] += (signed_imm * 4) - 4;
            }
            break;
        }

        // =================================================================
        // INSTRUÇÕES TIPO J (Controle de Fluxo com Imediato de 26 bits)
        // =================================================================
        case 0x17: { // JMP imm26
            uint32_t imm26 = instruction & 0x3FFFFFF;
            registers[15] = imm26;
            break;
        }

        case 0x18: { // CALL imm26 (Chama função)
            uint32_t imm26 = instruction & 0x3FFFFFF;
            registers[14] -= 4;
            memory.write32(registers[14], registers[15]);
            registers[15] = imm26;
            break;
        }

        // =================================================================
        // INSTRUÇÕES TIPO U (Imediato Superior de 22 bits)
        // =================================================================
        case 0x19: // PUSH rx (Empilha registrador de forma manual)
            registers[14] -= 4;
            memory.write32(registers[14], registers[rx]);
            break;

        case 0x1A: // POP rx (Desempilha registrador de forma manual)
            if (rx != 0) registers[rx] = memory.read32(registers[14]);
            registers[14] += 4;
            break;

        case 0x1B: // INC rx (Desempilha registrador de forma manual)
            if (rx != 0) registers[rx] = memory.read32(registers[14]);
            registers[14] += 4;
            break;

        case 0x1C: // DEC rx (Desempilha registrador de forma manual)
            if (rx != 0) registers[rx] = memory.read32(registers[14]);
            registers[14] += 4;
            break;

        case 0x1D: // NOT rx (Desempilha registrador de forma manual)
            if (rx != 0) registers[rx] = memory.read32(registers[14]);
            registers[14] += 4;
            break;

        case 0x1E: // RET rx (Desempilha registrador de forma manual)
            if (rx != 0) registers[rx] = memory.read32(registers[14]);
            registers[14] += 4;
            break;

        // =================================================================
        // INSTRUÇÕES TIPO S (Gráficos, Teclado e Pilha Manual: bits 13..0)
        // =================================================================
        

        default:
            std::cerr << "Erro: Opcode não implementado/inválido: 0x" << std::hex << (int)opcode << std::endl;
            exit(1);
    }
}