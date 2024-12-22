#ifndef IPROGRAM_LOADER_HPP_
#define IPROGRAM_LOADER_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace ploader {

enum class PloaderError {
    kOk            = 0,
    kInternalError = 1,
    kCantLoadBin   = 2,
    kWrongTarget   = 3,
};

struct LoadingSection {
    size_t start_addr;
    size_t end_addr;
    uint8_t* data;
};

class IProgramLoader {
  protected:
    std::vector<LoadingSection> lsections;
    size_t program_entry_point_;
  public:
    virtual PloaderError Init(const std::string& program_path) = 0;
    virtual ~IProgramLoader() = default;

    virtual const uint8_t* GetBinIndex(size_t index) const = 0;
    virtual size_t GetSizeIndex(size_t index) const = 0;
    virtual size_t GetStartAddrIndex(size_t index) const = 0;
    virtual size_t GetEndAddrIndex(size_t index) const = 0;
    virtual size_t GetEntryPoint() const = 0;
    virtual size_t GetNLSections() const = 0;
};

const char* PloaderErrorToStr(PloaderError err);

}; // namespace ploader

#endif // IPROGRAM_LOADER_HPP_
