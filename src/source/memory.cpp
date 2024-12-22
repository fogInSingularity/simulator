#include "memory.hpp"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cassert>
#include <cstring>

#include "log_helper.hpp"

// Memory ---------------------------------------------------------------------

void sim::Memory::Dump(size_t start_addr, size_t end_addr) const {
    LogFunctionEntry();

    LogVar(start_addr);

    const size_t kNOctets = 16;
    size_t last_addr = 0;
    for (size_t addr = start_addr; addr + kNOctets < end_addr; addr += kNOctets) {
        spdlog::info("current_addr: 0x{:x}", addr);
        for (size_t i = 0; i < kNOctets; i++) {
            spdlog::info("{:2x}", memory_[addr + i]); // NOTE may be i should fix log
        }
        
        spdlog::info("");

        last_addr = addr + kNOctets;
    }

    spdlog::info("current_addr: 0x{:x}", last_addr);
    for (size_t i = last_addr; i < end_addr; i++) {
        spdlog::info("{:2x} ", memory_[i]); // NOTE may be i should fix log
    }

    LogVar(end_addr);
}

uint32_t sim::Memory::ReadFromMemory32b(const MemAddress address) const {
    LogFunctionEntry();
   
    LogVar(address);

    if (address % sizeof(uint32_t) != 0) {
        spdlog::warn("Unaligned address memory access");
    }

    return *reinterpret_cast<uint32_t*>(memory_ + address);
}

uint16_t sim::Memory::ReadFromMemory16b(const MemAddress address) const {
    LogFunctionEntry();
   
    LogVar(address);

    if (address % sizeof(uint32_t) != 0) {
        spdlog::warn("Unaligned address memory access");
    }

    return *reinterpret_cast<uint16_t*>(memory_ + address);
}

uint8_t sim::Memory::ReadFromMemory8b(const MemAddress address) const {
    LogFunctionEntry();
   
    LogVar(address);

    if (address % sizeof(uint32_t) != 0) {
        spdlog::warn("Unaligned address memory access");
    }

    return *reinterpret_cast<uint8_t*>(memory_ + address);
}

void sim::Memory::WriteToMemory32b(const uint32_t data, const MemAddress address) {
    LogFunctionEntry();

    LogVar(data);
    LogVar(address);

    if (address % sizeof(uint32_t) != 0) {
        spdlog::warn("Unaligned address memory access");
    }

    *reinterpret_cast<uint32_t*>(memory_ + address) = data;
}

void sim::Memory::WriteToMemory16b(const uint16_t data, const MemAddress address) {
    LogFunctionEntry();

    LogVar(data);
    LogVar(address);

    if (address % sizeof(uint32_t) != 0) {
        spdlog::warn("Unaligned address memory access");
    }

    *reinterpret_cast<uint16_t*>(memory_ + address) = data;
}

void sim::Memory::WriteToMemory8b(const uint8_t data, const MemAddress address) {
    LogFunctionEntry();

    LogVar(data);
    LogVar(address);

    if (address % sizeof(uint32_t) != 0) {
        spdlog::warn("Unaligned address memory access");
    }

    *reinterpret_cast<uint8_t*>(memory_ + address) = data;
}

void sim::Memory::MapToMemory(const uint8_t* data_to_map, size_t start_addr, size_t end_addr) {
    assert(data_to_map != nullptr);

    spdlog::debug("Call to memory map: from 0x{:x} to 0x{:x}", start_addr, end_addr);

    std::memcpy(memory_ + start_addr, data_to_map, end_addr - start_addr);
}

size_t sim::Memory::GetMemorySize() const {
    LogFunctionEntry();

    LogVar(memory_size_);

    return memory_size_;
}

uint8_t* sim::Memory::GetData() {
    LogFunctionEntry();

    return memory_;
}

