#include <cassert>
#include <cstdlib>
#include <iostream>

#include "log_helper.hpp"

#include "elf_loader.hpp"
#include "sim.hpp"

int main(const int argc, const char* const argv[]) {
    auto logger = spdlog::basic_logger_mt("simulator", "simulator.log", true);
    spdlog::set_default_logger(logger);

#if defined (NDEBUG)
    spdlog::set_level(spdlog::level::info);
#else // DEBUG
    spdlog::flush_on(spdlog::level::trace);
    spdlog::set_level(spdlog::level::debug);
#endif // NDEBUG

    if (argc < 2) {
        std::cerr << "[Error]: Executable file was not passed" << std::endl;
        spdlog::error("Executable file was not passed");
        return EXIT_FAILURE;
    }

    ploader::ElfLoader elf_loader;
    ploader::PloaderError load_error = elf_loader.Init(argv[1]);
    if (load_error != ploader::PloaderError::kOk) {
        std::cerr << "[Error]: cant load executable," << ploader::PloaderErrorToStr(load_error) << std::endl;
        spdlog::error("Cant load elf", ploader::PloaderErrorToStr(load_error));

        return EXIT_FAILURE;
    }

    spdlog::info("Elf loaded");
    
    sim::Simulator simulator(elf_loader);

    simulator.Execute();

    return EXIT_SUCCESS;
}

