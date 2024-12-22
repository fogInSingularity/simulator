#ifndef LOG_HELPER_HPP_
#define LOG_HELPER_HPP_

#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#ifndef LogFunctionEntry
// #define LogFunctionEntry() spdlog::debug("{} function entry")
#define LogFunctionEntry() ;
#endif // LogFunctionEntry

#ifndef LogVar
#define LogVar(var_) spdlog::trace("{}: {}", #var_, var_)
#endif // LogVar

#ifndef LogVarX
#define LogVarX(xvar_) spdlog::trace("{}: {:x}", #xvar_, xvar_)
#endif // LogVarX

#ifndef TO_STR
#define TO_STR(x_to_str_) #x_to_str_
#endif // TO_STR

#endif // LOG_HELPER_HPP_
