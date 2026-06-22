#pragma once
#include "../memory/memory.h"

class CPU {
private:
    uint32_t registers[16];
    
    // Atalhos para legibilidade
    const int SP = 14;
    const int PC = 15;

public:
    CPU();

    void setRegister(int reg, uint32_t value);

    uint32_t getRegister(int reg) const;

    // Executa o ciclo de uma única instrução
    void step(Memory& memory);
};