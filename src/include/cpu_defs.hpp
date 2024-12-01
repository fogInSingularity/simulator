#ifndef CPU_DEFS_HPP_
#define CPU_DEFS_HPP_

#include <cstdint>
#include <cstddef>

using Register = uint32_t;
using IRegister = int32_t;
using Address = uint32_t;

static const size_t kNumberOfRegisters = 32; 

const size_t kRetAddrIndex = 1;
const size_t kStackPointerIndex = 2;

const size_t kStartingReturnAddress = 0xDED;

enum class CpuErrors {
    kOk            = 0,
    kBadAlloc      = 1,
    kBadExecutable = 2,
};

const char* CpuErrorsToStr(CpuErrors error);

#endif // CPU_DEFS_HPP_
