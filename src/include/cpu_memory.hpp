#ifndef CPU_MEMORY_HPP_
#define CPU_MEMORY_HPP_

#include <cstdint>
#include <cstddef>

#include "cpu_defs.hpp"

class Memory {
  private:
    uint8_t* memory_;
    size_t executable_size_;
  public:
    Memory(const char* executable_name);
    ~Memory();

    void Dump() const;

    Register ReadFromMemory(Address address) const;
    void WriteToMemory(Register data, Address address);
    size_t GetExecutableSize() const;
};

#endif // CPU_MEMORY_HPP_
