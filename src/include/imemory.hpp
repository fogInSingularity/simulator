#ifndef IMEMORY_HPP_
#define IMEMORY_HPP_

#include <cstddef>
#include <cstdint>

namespace sim {

using MemAddress = uint32_t;

class IMemory {
  public:
    virtual void Init(size_t memory_size) = 0;
    virtual ~IMemory() = default;

    virtual void Dump(size_t start_addr, size_t end_addr) const = 0;

    virtual uint32_t ReadFromMemory32b(const MemAddress address) const = 0;
    virtual uint16_t ReadFromMemory16b(const MemAddress address) const = 0;
    virtual uint8_t  ReadFromMemory8b (const MemAddress address) const = 0;

    virtual void WriteToMemory32b(const uint32_t data, const MemAddress address) = 0;
    virtual void WriteToMemory16b(const uint16_t data, const MemAddress address) = 0;
    virtual void WriteToMemory8b (const uint8_t data, const MemAddress address) = 0;

    virtual void MapToMemory(const uint8_t* data_to_map, size_t start_addr, size_t end_addr) = 0;

    virtual size_t GetMemorySize() const = 0;
    virtual uint8_t* GetData() = 0;
};

}; // namespace sim

#endif // IMEMORY_HPP_
