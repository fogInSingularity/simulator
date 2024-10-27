#include "cpu_memory.hpp"

#include <cstdint>
#include <cstdio>
#include <cassert>

#include "logger.hpp"

#include "cpu_defs.hpp"

// static ---------------------------------------------------------------------

static size_t GetFileSize(FILE* file);

// Memory ---------------------------------------------------------------------

Memory::Memory(const char* const executable_name) 
{
    assert(executable_name != nullptr);

    LogFunctionEntry();

    memory_ = new uint8_t[kMemorySize]{};

    FILE* executable_file = fopen(executable_name, "rb");
    if (executable_file == nullptr) {
        delete memory_;
        memory_ = nullptr;
        Log("cant open %s\n", TO_STR(executable_file));
        LogVariable("%s", executable_name);

        throw CpuErrors::kBadExecutable;
    }

    size_t file_size = GetFileSize(executable_file);
    LogVariable("%lu", file_size);
    executable_size_ = file_size;

    if (file_size >= kMemorySize) {
        delete memory_;
        memory_ = nullptr;
        fclose(executable_file);
        throw CpuErrors::kBadExecutable;
    }
    size_t readed = fread(memory_, sizeof(uint8_t), file_size, executable_file);
    LogVariable("%lu", readed);

    fclose(executable_file);
}

Memory::~Memory() {
    LogFunctionEntry();

    delete[] memory_;
}

void Memory::Dump() const {
    LogFunctionEntry();

    for (size_t i = 0; i < kMemorySize; i++) {
        // FIXME 
    }
}

uint32_t Memory::ReadFromMemory32b(const MemAddress address) const {
    LogFunctionEntry();
   
    LogVariable("%u", address);

    return *reinterpret_cast<uint32_t*>(memory_ + address);
}

uint16_t Memory::ReadFromMemory16b(const MemAddress address) const {
    LogFunctionEntry();
   
    LogVariable("%u", address);

    return *reinterpret_cast<uint16_t*>(memory_ + address);
}

uint8_t Memory::ReadFromMemory8b(const MemAddress address) const {
    LogFunctionEntry();
   
    LogVariable("%u", address);

    return *reinterpret_cast<uint8_t*>(memory_ + address);
}

void Memory::WriteToMemory32b(const uint32_t data, const MemAddress address) {
    LogFunctionEntry();

    LogVariable("%u", data);
    LogVariable("%u", address);

    *reinterpret_cast<uint32_t*>(memory_ + address) = data;
}

void Memory::WriteToMemory16b(const uint16_t data, const MemAddress address) {
    LogFunctionEntry();

    LogVariable("%u", data);
    LogVariable("%u", address);

    *reinterpret_cast<uint16_t*>(memory_ + address) = data;
}

void Memory::WriteToMemory8b(const uint8_t data, const MemAddress address) {
    LogFunctionEntry();

    LogVariable("%u", data);
    LogVariable("%u", address);

    *reinterpret_cast<uint8_t*>(memory_ + address) = data;
}

size_t Memory::GetExecutableSize() const {
    LogFunctionEntry();

    return executable_size_;
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
