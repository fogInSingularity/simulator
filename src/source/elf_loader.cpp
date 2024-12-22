#include "elf_loader.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

#include "elfio/elf_types.hpp"
#include "elfio/elfio.hpp"

#include "log_helper.hpp"

#include "iprogram_loader.hpp"

ploader::PloaderError ploader::ElfLoader::Init(const std::string& program_path) {
    LogFunctionEntry();

    program_entry_point_ = 0;

    ELFIO::elfio elf;
    if (!elf.load(program_path)) {
        spdlog::error("Cant load binary file");
        return ploader::PloaderError::kCantLoadBin;
    }
    
    bool is_correct_bitness   = elf.get_class()    == ELFIO::ELFCLASS32;
    bool is_correct_endianess = elf.get_encoding() == ELFIO::ELFDATA2LSB; 
    bool is_correct_arch      = elf.get_machine()  == ELFIO::EM_RISCV;

    if (!is_correct_bitness || !is_correct_endianess || !is_correct_arch) {
        return ploader::PloaderError::kWrongTarget;
    }

    for (size_t i = 0; i < elf.segments.size(); i++) {
        const auto* segment = elf.segments[i];

        if (segment->get_type() == ELFIO::PT_LOAD) {
            spdlog::debug("Found executable code");
            size_t start_addr = segment->get_virtual_address();
            size_t end_addr = start_addr + segment->get_memory_size();

            size_t ls_size = end_addr - start_addr;
            uint8_t* ls_bin = new uint8_t[ls_size];
            size_t ls_start_addr = start_addr;
            size_t ls_end_addr= end_addr;

            std::memcpy(ls_bin, segment->get_data(), segment->get_file_size());

            LoadingSection lsection = {
                .start_addr = ls_start_addr,    
                .end_addr = ls_end_addr,
                .data = ls_bin,
            };

            lsections.push_back(lsection);
        }
    }

    program_entry_point_ = elf.get_entry();

    return ploader::PloaderError::kOk;
}

const uint8_t* ploader::ElfLoader::GetBinIndex(size_t index) const {
    return lsections[index].data;
}

size_t ploader::ElfLoader::GetSizeIndex(size_t index) const {
    return lsections[index].end_addr - lsections[index].start_addr;
}

size_t ploader::ElfLoader::GetStartAddrIndex(size_t index) const {
    return lsections[index].start_addr;   
}

size_t ploader::ElfLoader::GetEndAddrIndex(size_t index) const {
    return lsections[index].end_addr;
}

size_t ploader::ElfLoader::GetEntryPoint() const {
    return program_entry_point_;
}

size_t ploader::ElfLoader::GetNLSections() const {
    return lsections.size();
}
