#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <cstdint>
#include <cstddef>

#include "imemory.hpp"

namespace sim {

class Memory : public IMemory {
  private:
    uint8_t* memory_;
    size_t memory_size_;
  public:
    void Init(size_t memory_size) override {
        memory_size_ = memory_size;
        memory_ = new uint8_t[memory_size_]{};
    }
    ~Memory() override { delete[] memory_; };

    void Dump(size_t start_addr, size_t end_addr) const override;

    uint32_t ReadFromMemory32b(const MemAddress address) const override;
    uint16_t ReadFromMemory16b(const MemAddress address) const override;
    uint8_t  ReadFromMemory8b (const MemAddress address) const override;

    void WriteToMemory32b(const uint32_t data, const MemAddress address) override;
    void WriteToMemory16b(const uint16_t data, const MemAddress address) override;
    void WriteToMemory8b (const uint8_t data, const MemAddress address) override;

    void MapToMemory(const uint8_t* data_to_map, size_t start_addr, size_t end_addr) override;

    size_t GetMemorySize() const override;
    uint8_t* GetData() override;
};

} // namespace sim


#endif // MEMORY_HPP_
