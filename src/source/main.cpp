#include "cpu.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "elfio/elf_types.hpp"
#include "elfio/elfio.hpp"
#include "elfio/elfio_dump.hpp"

#include "logger.hpp"

int main(const int argc, const char* const argv[]) {
    LoggingStatus logging_status = LoggingSetup("simulator.log");
    assert(logging_status == LoggingStatus::kOk);

    if (argc < 2) {
        std::cerr << "Error: executable file was not passed" << std::endl;
        return EXIT_FAILURE;
    }

    ELFIO::elfio elf;
    if (!elf.load(argv[1])) {
        std::cerr << "Error: cant load executable" << std::endl;
        return EXIT_FAILURE;
    }

    // ELFIO::dump::header(std::cout, elf_reader);
    // ELFIO::dump::section_headers(std::cout, elf_reader);
    // ELFIO::dump::segment_headers(std::cout, elf_reader);
    // ELFIO::dump::symbol_tables(std::cout, elf_reader);
    // ELFIO::dump::notes(std::cout, elf_reader);
    // ELFIO::dump::modinfo(std::cout, elf_reader);
    // ELFIO::dump::dynamic_tags(std::cout, elf_reader);
    // ELFIO::dump::section_datas(std::cout, elf_reader);
    // ELFIO::dump::segment_datas(std::cout, elf_reader);

    // for (size_t i = 0; i < elf_reader.segments.size(); i++) {
    //     const auto* segment = elf_reader.segments[i];
    //     if (segment->get_type() == ELFIO::PT_LOAD) {
    //         std::cout << "Loading segment " << i << ": " 
    //                   << "offset=" << segment->get_offset()
    //                   << ", vaddr=0x" << std::hex << segment->get_virtual_address()
    //                   << ", memsize=" << segment->get_memory_size()
    //                   << ", filesize=" << segment->get_file_size() << std::endl;
    //     }  
    // }


    // return 0;

    Cpu cpu(elf);

    cpu.Dump();

    cpu.ExecuteBin();

    cpu.Dump();
}

void LogElfFile(const ELFIO::elfio elf_reader) {
    LogFunctionEntry();

}
