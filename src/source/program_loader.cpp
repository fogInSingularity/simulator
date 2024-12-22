#include "iprogram_loader.hpp"

#include <assert.h>

const char* ploader::PloaderErrorToStr(PloaderError err) {
    switch (err) {
        case ploader::PloaderError::kOk:            return "no error";
        case ploader::PloaderError::kInternalError: return "internal error";
        case ploader::PloaderError::kCantLoadBin:   return "cant load bin";
        case ploader::PloaderError::kWrongTarget:   return "wrong target";
        default:
            assert(0 && "unknown enum value");
            return "<unknown enum value>";
    }
}
