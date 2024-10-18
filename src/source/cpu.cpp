#include "cpu.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>

#include "cpu_defs.hpp"
#include "cpu_memory.hpp"
#include "instructions.hpp"
#include "logging.h"
#include "instruction_semantic.hpp"

// static ---------------------------------------------------------------------



// Cpu private ----------------------------------------------------------------

Register Cpu::Fetch() {
    LogFunctionEntry();

    if (pc_ >= kMemorySize) {
        is_finished_ = true;
        return 0;
    }

    Register read_mem = memory_.ReadFromMemory(pc_);
    LogVariable("%u", read_mem);

    return read_mem;
}

InstructionOpcodes Cpu::Decode(const Register encoded_instr) const {
    LogFunctionEntry();

    InstructionOpcodes opcode_instr_value = static_cast<InstructionOpcodes>(encoded_instr & kOpcodeMask); 

    LogVariable("%x", static_cast<Register>(opcode_instr_value));

    return opcode_instr_value;
}

InstructionError Cpu::Execute(const Register instr, const InstructionOpcodes opcode) {
    LogFunctionEntry();

    LogVariable("%x", instr);
    LogVariable("%x", static_cast<Register>(opcode));

    switch (opcode) {
        case InstructionOpcodes::kLui:            return InstructionLui(this, instr);
        case InstructionOpcodes::kAuipc:          return InstructionAuipc(this, instr);
        case InstructionOpcodes::kJal:            return InstructionJal(this, instr);
        case InstructionOpcodes::kJalr:           return InstructionJalr(this, instr);
        case InstructionOpcodes::kBranchInstr:    return InstructionBranchInstr(this, instr);
        case InstructionOpcodes::kLoadInstr:      return InstructionLoadInstr(this, instr);
        case InstructionOpcodes::kStoreInstr:     return InstructionStoreInstr(this, instr);
        case InstructionOpcodes::kArithmImmInstr: return InstructionArithmImmInstr(this, instr);
        case InstructionOpcodes::kArithmRegInstr: return InstructionArithmRegInstr(this, instr);
        case InstructionOpcodes::kFenceInstr:     return InstructionFenceInstr(this, instr);
        case InstructionOpcodes::kSystemInstr:    return InstructionSystemInstr(this, instr);

        default:
            Log("unknown instrucion\n");
            return InstructionError::kUnknownInstruction;
    }
}

// Cpu public -----------------------------------------------------------------

Cpu::Cpu(const char* const executable_name) : 
    memory_(executable_name)
{
    assert(executable_name != nullptr);

    LogFunctionEntry();

    memset(registers_, 0, sizeof(registers_));
    pc_ = 0;
    is_finished_ = false;
}

Register Cpu::GetPc() const {
    LogFunctionEntry();

    return pc_;
}

void Cpu::SetPc(const Register new_pc) {
    LogFunctionEntry();

    pc_ = new_pc;
}

Register Cpu::GetRegisterValue(const size_t register_id) const {
    assert(register_id < kNumberOfRegisters);

    if (register_id == 0) {
        return 0;
    }

    return registers_[register_id];
}

void Cpu::SetRegisterValue(const size_t register_id, const Register new_value) {
    assert(register_id < kNumberOfRegisters);

    if (register_id == 0) { // NOTE mb use constant for x0
        return ;
    } 

    registers_[register_id] = new_value;
}

bool Cpu::GetIsFinished() const {
    return is_finished_;    
}

void Cpu::SetIsFinished(const bool is_finished) {
    is_finished_ = is_finished;
}

Register Cpu::ReadFromMemory(const Address address) const {
    LogFunctionEntry();

    return memory_.ReadFromMemory(address);
}

void Cpu::WriteToMemory(const Register data, const Address address) {
    LogFunctionEntry();

    memory_.WriteToMemory(data, address);
}

void Cpu::Dump() const {
    LogFunctionEntry();

    Log("\n\n");

    LogVariable("%u", pc_);

    for (size_t i = 0; i < kNumberOfRegisters; i++) {
        Log("Register x%lu: %u\n", i, registers_[i]);
    } 

    Log("Has finished: %s\n", is_finished_ ? "true" : "false");

    memory_.Dump();

    Log("\n\n");
}

void Cpu::ExecuteBin() {
    LogFunctionEntry();
 
    while (!is_finished_ && pc_ < memory_.GetExecutableSize()) {
        Register instr = Fetch();
        InstructionOpcodes instruction = Decode(instr);
        InstructionError instr_error = Execute(instr, instruction);
        LogVariable("%u", static_cast<Register>(instr_error));

        // Dump(); // FIXME
    }
}
