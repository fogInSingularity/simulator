#include "cpu.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "elfio/elfio.hpp"

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

    Cpu cpu(elf);

    cpu.Dump();

    cpu.ExecuteBin();

    cpu.Dump();
}

