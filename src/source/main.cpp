#include "cpu.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "logger.hpp"

int main(const int argc, const char* const argv[]) {
    LoggingStatus logging_status = LoggingSetup("simulator.log");
    assert(logging_status == LoggingStatus::kOk);

    if (argc < 2) {
        std::cerr << "Error: executable file was not passed" << std::endl;
        return EXIT_FAILURE;
    }

    Cpu cpu(argv[1]);

    cpu.Dump();

    cpu.ExecuteBin();

    cpu.Dump();
}
