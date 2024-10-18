#ifndef CPU_DEFS_HPP_
#define CPU_DEFS_HPP_

#include <cstdint>
#include <cstddef>

using Register = uint32_t;
using IRegister = int32_t;
using Address = uint32_t;

static const size_t kNumberOfRegisters = 32; 
static const size_t kMemorySize = 0x400'000;

enum class CpuErrors {
    kOk            = 0,
    kBadAlloc      = 1,
    kBadExecutable = 2,
};

const char* CpuErrorsToStr(CpuErrors error);

#endif // CPU_DEFS_HPP_
