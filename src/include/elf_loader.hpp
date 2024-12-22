#ifndef ELF_LOADER_HPP_
#define ELF_LOADER_HPP_

#include <cstdint>
#include <string>
#include <vector>

#include "iprogram_loader.hpp"

namespace ploader {

class ElfLoader: public IProgramLoader {
  public:
    PloaderError Init(const std::string& program_path) override;
    ~ElfLoader() {
        for (size_t ls_i = 0; ls_i < lsections.size(); ls_i++) {
            LoadingSection ls = lsections[ls_i];
            delete[] ls.data;
        }
    };

    const uint8_t* GetBinIndex(size_t index) const override;
    size_t GetSizeIndex(size_t index) const override;
    size_t GetStartAddrIndex(size_t index) const override;
    size_t GetEndAddrIndex(size_t index) const override;
    size_t GetEntryPoint() const override;
    size_t GetNLSections() const override;
};

}; // namespace ploader

#endif // ELF_LOADER_HPP_
