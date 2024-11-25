#ifndef CPU_MEMORY_HPP_
#define CPU_MEMORY_HPP_

#include <cstdint>
#include <cstddef>
#include <vector>

#include "elfio/elfio.hpp"

using MemAddress = uint32_t;

class Memory {
  private:
    // uint8_t* memory_;
    uint8_t* memory_;
    size_t memory_size_;
    size_t executable_start_;
    size_t executable_end_;
  public:
    Memory(const ELFIO::elfio& elf);
    ~Memory();

    void Dump(size_t start_addr, size_t end_addr) const;

    uint32_t ReadFromMemory32b(const MemAddress address) const;
    uint16_t ReadFromMemory16b(const MemAddress address) const;
    uint8_t  ReadFromMemory8b (const MemAddress address) const;

    void WriteToMemory32b(const uint32_t data, const MemAddress address);
    void WriteToMemory16b(const uint16_t data, const MemAddress address);
    void WriteToMemory8b (const uint8_t data, const MemAddress address);

    size_t GetExecutableStart() const;
    size_t GetExecutableEnd() const;
    size_t GetMemorySize() const;
};

#endif // CPU_MEMORY_HPP_
