#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>

class Memory {
private:
    std::vector<uint8_t> ram;
    const uint32_t MEM_SIZE = 16 * 1024 * 1024; // 16 MB

public:
    Memory();

    const uint32_t getMemSize();

    // Lê 32 bits da memória (4 bytes) tratando o alinhamento e Big-Endian
    uint32_t read32(uint32_t address);

    // Escreve 32 bits na memória
    void write32(uint32_t address, uint32_t value);

    // Método auxiliar para expor o ponteiro do framebuffer para a SDL
    uint8_t* getFramebufferPtr();
};