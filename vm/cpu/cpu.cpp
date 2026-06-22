#include "cpu.h"
#include <iostream>

CPU::CPU() {
    for(int i = 0; i < 16; i++) registers[i] = 0;
    registers[PC] = 0x00000000; // Começa no início da memória
    registers[SP] = 0x01000000; // Começa no topo da Stack (16MB)
}

void CPU::setRegister(int reg, uint32_t value) {
    if (reg == 0) return; // R0 é sempre constante zero!
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
    // O formato depende da especificação. Supondo que o Opcode ocupe os 6 bits superiores:
    uint8_t opcode = (instruction >> 26) & 0x3F;
    uint8_t rx     = (instruction >> 21) & 0x1F;
    uint8_t ry     = (instruction >> 16) & 0x1F;
    uint8_t rz     = (instruction >> 11) & 0x1F;
    uint16_t imm   = instruction & 0xFFFF; // Imediato de 16 bits

    // 3. Execução (Execute)
    switch(opcode) {
        case 0x01: { // Exemplo fictício: ADD Rx, Ry, Rz (Rx = Ry + Rz)
            uint32_t valY = getRegister(ry);
            uint32_t valZ = getRegister(rz);
            setRegister(rx, valY + valZ);
            break;
        }
        case 0x02: { // Exemplo: ADDI Rx, Ry, Imm (Rx = Ry + Imm)
            uint32_t valY = getRegister(ry);
            // Lembre-se de fazer extensão de sinal se o imediato for com sinal!
            int32_t sign_extended_imm = (int16_t)imm; 
            setRegister(rx, valY + sign_extended_imm);
            break;
        }
        case 0x03: { // Exemplo: LOAD Rx, Ry, Imm (Rx = Memoria[Ry + Imm])
            uint32_t target_addr = getRegister(ry) + (int16_t)imm;
            uint32_t val = memory.read32(target_addr);
            setRegister(rx, val);
            break;
        }
        // ... Você vai adicionar um 'case' para cada instrução da especificação (RECT, GKEY, etc.)
        
        default:
            std::cerr << "Instrução desconhecida/inválida: " << (int)opcode << std::endl;
            exit(1);
    }
}