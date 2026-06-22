#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>

class Memory {
private:
    std::vector<uint8_t> ram;
    const uint32_t MEM_SIZE = 16 * 1024 * 1024; // 16 MB

public:
    Memory() { ram.resize(MEM_SIZE, 0); }

    // Lê 32 bits da memória (4 bytes) tratando o alinhamento e Big-Endian
    uint32_t read32(uint32_t address) {
        if (address % 4 != 0) throw std::runtime_error("Alignment Error");
        if (address + 3 >= MEM_SIZE) throw std::runtime_error("Invalid Address");

        // Big-Endian: combina os 4 bytes
        return (ram[address] << 24)     | 
               (ram[address + 1] << 16) | 
               (ram[address + 2] << 8)  | 
               (ram[address + 3]);
    }

    // Escreve 32 bits na memória
    void write32(uint32_t address, uint32_t value) {
        if (address % 4 != 0) throw std::runtime_error("Alignment Error");
        if (address + 3 >= MEM_SIZE) throw std::runtime_error("Invalid Address");

        // Big-Endian: quebra o valor de 32 bits em 4 bytes
        ram[address]     = (value >> 24) & 0xFF;
        ram[address + 1] = (value >> 16) & 0xFF;
        ram[address + 2] = (value >> 8)  & 0xFF;
        ram[address + 3] = value & 0xFF;
    }

    // Método auxiliar para expor o ponteiro do framebuffer para a SDL
    uint8_t* getFramebufferPtr() {
        return &ram[0x00FB4000];
    }
};