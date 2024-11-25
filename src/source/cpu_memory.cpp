#include "cpu_memory.hpp"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cassert>
#include <iterator>

#include "elfio/elf_types.hpp"
#include "elfio/elfio.hpp"

#include "logger.hpp"

#include "cpu_defs.hpp"

// static ---------------------------------------------------------------------

static size_t GetFileSize(FILE* file);

// Memory ---------------------------------------------------------------------

Memory::Memory(const ELFIO::elfio& elf) {
    LogFunctionEntry();

    memory_ = 0;
    memory_size_ = 0;

    // FIXME make better memory mapping 

    for (size_t i = 0; i < elf.segments.size(); i++) {
        const auto* segment = elf.segments[i];

        if (segment->get_type() == ELFIO::PT_LOAD) {
            Log("found pt_load segment: %lu\n", i);

            size_t start_addr = segment->get_virtual_address();
            size_t end_addr = start_addr + segment->get_memory_size();

            LogVariable("%lx", start_addr);
            LogVariable("%lx", end_addr);

            memory_size_ = end_addr * 2;
            memory_ = new uint8_t[memory_size_];
            executable_start_ = start_addr;
            executable_end_ = end_addr;

            std::memcpy(&memory_[start_addr], segment->get_data(), segment->get_file_size());
        }
    }

    // memory_ = new uint8_t[kMemorySize]{};

    // FILE* executable_file = fopen(executable_name, "rb");
    // if (executable_file == nullptr) {
    //     delete memory_;
    //     memory_ = nullptr;
    //     Log("cant open %s\n", TO_STR(executable_file));
    //     LogVariable("%s", executable_name);

    //     throw CpuErrors::kBadExecutable;
    // }

    // size_t file_size = GetFileSize(executable_file);
    // LogVariable("%lu", file_size);
    // executable_size_ = file_size;

    // if (file_size >= kMemorySize) {
    //     delete memory_;
    //     memory_ = nullptr;
    //     fclose(executable_file);
    //     throw CpuErrors::kBadExecutable;
    // }
    // size_t readed = fread(memory_, sizeof(uint8_t), file_size, executable_file);
    // LogVariable("%lu", readed);

    // fclose(executable_file);
    //
    //
}

Memory::~Memory() {
    LogFunctionEntry();

    delete[] memory_;
}

void Memory::Dump(size_t start_addr, size_t end_addr) const {
    LogFunctionEntry();

    LogVariable("0x%lx", start_addr);

    const size_t kNOctets = 16;
    size_t last_addr = 0;
    for (size_t addr = start_addr; addr + kNOctets < end_addr; addr += kNOctets) {
        RawWriteToLog("current_addr: 0x%lx\t", addr);
        for (size_t i = 0; i < kNOctets; i++) {
            RawWriteToLog("%2x ", memory_[addr + i]); // NOTE may be i should fix log
        }
        Log("\n");

        last_addr = addr + kNOctets;
    }

    RawWriteToLog("current_addr: 0x%lx\t", last_addr);
    for (size_t i = last_addr; i < end_addr; i++) {
        RawWriteToLog("%2x ", memory_[i]); // NOTE may be i should fix log
    }
    Log("\n");


    LogVariable("0x%lx", end_addr);
}

uint32_t Memory::ReadFromMemory32b(const MemAddress address) const {
    LogFunctionEntry();
   
    LogVariable("0x%x", address);

    return *reinterpret_cast<uint32_t*>(memory_ + address);
}

uint16_t Memory::ReadFromMemory16b(const MemAddress address) const {
    LogFunctionEntry();
   
    LogVariable("0x%x", address);

    return *reinterpret_cast<uint16_t*>(memory_ + address);
}

uint8_t Memory::ReadFromMemory8b(const MemAddress address) const {
    LogFunctionEntry();
   
    LogVariable("0x%x", address);

    return *reinterpret_cast<uint8_t*>(memory_ + address);
}

void Memory::WriteToMemory32b(const uint32_t data, const MemAddress address) {
    LogFunctionEntry();

    LogVariable("%u", data);
    LogVariable("0x%x", address);

    *reinterpret_cast<uint32_t*>(memory_ + address) = data;
}

void Memory::WriteToMemory16b(const uint16_t data, const MemAddress address) {
    LogFunctionEntry();

    LogVariable("%u", data);
    LogVariable("0x%x", address);

    *reinterpret_cast<uint16_t*>(memory_ + address) = data;
}

void Memory::WriteToMemory8b(const uint8_t data, const MemAddress address) {
    LogFunctionEntry();

    LogVariable("%u", data);
    LogVariable("0x%x", address);

    *reinterpret_cast<uint8_t*>(memory_ + address) = data;
}

size_t Memory::GetExecutableStart() const {
    LogFunctionEntry();

    LogVariable("0x%lx", executable_start_);

    return executable_start_;
}

size_t Memory::GetExecutableEnd() const {
    LogFunctionEntry();

    LogVariable("0x%lx", executable_end_);

    return executable_end_;
}

size_t Memory::GetMemorySize() const {
    LogFunctionEntry();

    LogVariable("0x%lx", memory_size_);

    return memory_size_;
}

// static ---------------------------------------------------------------------

static size_t GetFileSize(FILE* file) {
    assert(file != nullptr);

    LogFunctionEntry();

    fseek(file, 0, SEEK_END);
    
    bool error_flag = false;
    int64_t file_size = ftell(file);
    if (file_size < 0) {
        error_flag = true;
    } 
    rewind(file);

    return !error_flag
                ? (size_t)file_size
                : 0;
}
