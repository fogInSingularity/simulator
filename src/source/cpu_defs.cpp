#include "cpu_defs.hpp"

#include <cassert>

#include "logger.hpp"

#ifndef TOSTR
#define TOSTR(x) #x
#endif // TOSTR

const char* CpuErrorsToStr(CpuErrors error) {
    LogFunctionEntry();

    switch (error) {
        case CpuErrors::kOk:            return TOSTR(CpuErrors::kOk);
        case CpuErrors::kBadAlloc:      return TOSTR(CpuErrors::kBadAlloc);
        case CpuErrors::kBadExecutable: return TOSTR(CpuErrors::kBadExecutable);
        default: assert(0 && "unknown CpuErrors value"); return "< unknown CpuErrors value >";
    }
}
