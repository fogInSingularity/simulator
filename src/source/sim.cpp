#include "sim.hpp"

#include <iostream>

#include "log_helper.hpp"

#include "decode.hpp"
#include "imemory.hpp"
#include "instructions.hpp"
#include "sim_cfg.hpp"
#include "cpu_defs.hpp"
#include "iprogram_loader.hpp"

sim::Simulator::Simulator(const ploader::IProgramLoader& ploader) 
{
    LogFunctionEntry();

    memory_.Init(kMemorySize);
    for (size_t index_ls = 0; index_ls < ploader.GetNLSections(); index_ls++) {   
        memory_.MapToMemory(ploader.GetBinIndex(index_ls), ploader.GetStartAddrIndex(index_ls), ploader.GetEndAddrIndex(index_ls));
    }

    cpu_.Init(ploader.GetEntryPoint(), &memory_);
}

void sim::Simulator::Execute() {
    LogFunctionEntry();

    cpu_.Dump();
    // Register cpu_pc = cpu_.GetPc();
    // LogVarX(cpu_pc);
    // memory_.Dump(cpu_pc - 32, cpu_pc + 32);

    while (!cpu_.GetIsFinished()) {
        spdlog::debug("Start of instruction execution");
        Register instr = FetchInstr();
        DecodedInstr dec_instr = Decode(instr);
        InstructionError err = cpu_.Execute(dec_instr);
        if (err != InstructionError::kOk) {
            spdlog::error("Error occurd while instruction execution");
        }
        spdlog::debug("End of instruction execution");
        // cpu_.Dump();
    }

    cpu_.Dump();
}

sim::Register sim::Simulator::FetchInstr() {
    LogFunctionEntry();

    return memory_.ReadFromMemory32b(cpu_.GetPc());
}
