#ifndef CPU_DEFS_HPP_
#define CPU_DEFS_HPP_

#include <cstdint>
#include <cstddef>

namespace sim {

static const size_t kNumberOfRegisters = 32; 

// const size_t kMachineZeroIndex  = 0;
// const size_t kRetAddrIndex      = 1;
// const size_t kStackPointerIndex = 2;

enum RegisterAliases {
    kMachineZero   = 0,
    kRetAddr       = 1,
    kStackPointer  = 2,
    kGlobalPointer = 3,
    kThreadPointer = 4,
    kTemporary0    = 5,
    kTemporary1    = 6,
    kTemporary2    = 7,
    kCalleeSaved0  = 8,
    kCalleeSaved1  = 9,
    kArgument0     = 10,
    kArgument1     = 11,
    kArgument2     = 12,
    kArgument3     = 13,
    kArgument4     = 14,
    kArgument5     = 15,
    kArgument6     = 16,
    kArgument7     = 17,
    // FIXME 
};

const size_t kStartingReturnAddress = 0xDED;

enum class CpuErrors {
    kOk            = 0,
    kBadAlloc      = 1,
    kBadExecutable = 2,
};

enum class InstructionError {
    kOk                 = 0,
    kUnknownInstruction = 1,
    kMisalignedAddress  = 2,
};

enum class TranslateError {
    kOk                 = 0,
    kUnknownInstruction = 1,
};

const size_t kSyscallIdRegister = RegisterAliases::kArgument7;

enum SyscallIds {
    kRead = 63,
    kWrite = 64,
    kExit = 93,
};

enum DefaultDesriptors {
    kStdin  = 0,
    kStdout = 1,
    kStderr = 2,
};

const char* CpuErrorsToStr(CpuErrors error);

} // namespace sim

#endif // CPU_DEFS_HPP_
