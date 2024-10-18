#include "cpu_memory.hpp"

#include <cstdint>
#include <cstdio>
#include <string>
#include <cassert>

#include "cpu_defs.hpp"
#include "logging.h"

// static ---------------------------------------------------------------------

static size_t GetFileSize(FILE* file);

// Memory ---------------------------------------------------------------------

Memory::Memory(const char* executable_name) 
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

Register Memory::ReadFromMemory(Address address) const {
    LogFunctionEntry();
   
    LogVariable("%u", address);

    return *reinterpret_cast<Register*>(memory_ + address);
}

void Memory::WriteToMemory(Register data, Address address) {
    LogFunctionEntry();

    LogVariable("%u", data);
    LogVariable("%u", address);

    *reinterpret_cast<Register*>(memory_ + address) = data;
}

size_t Memory::GetExecutableSize() const {
    return executable_size_;
}

// static ---------------------------------------------------------------------

static size_t GetFileSize(FILE* file) {
    assert(file != nullptr);

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
