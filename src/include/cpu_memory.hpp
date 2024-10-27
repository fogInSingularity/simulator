#ifndef CPU_MEMORY_HPP_
#define CPU_MEMORY_HPP_

#include <cstdint>
#include <cstddef>

using MemAddress = uint32_t;

class Memory {
  private:
    uint8_t* memory_;
    size_t executable_size_;
  public:
    Memory(const char* const executable_name);
    ~Memory();

    void Dump() const;

    uint32_t ReadFromMemory32b(const MemAddress address) const;
    uint16_t ReadFromMemory16b(const MemAddress address) const;
    uint8_t  ReadFromMemory8b (const MemAddress address) const;

    void WriteToMemory32b(const uint32_t data, const MemAddress address);
    void WriteToMemory16b(const uint16_t data, const MemAddress address);
    void WriteToMemory8b (const uint8_t data, const MemAddress address);

    size_t GetExecutableSize() const;
};

#endif // CPU_MEMORY_HPP_
