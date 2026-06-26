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

    uint32_t read32(uint32_t address);

    void write32(uint32_t address, uint32_t value);

    uint8_t* getFramebufferPtr();
};