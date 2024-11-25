#ifndef CPU_HPP_
#define CPU_HPP_

#include <cstdlib>

#include "elfio/elfio.hpp"

#include "cpu_defs.hpp"
#include "cpu_memory.hpp"
#include "instructions.hpp"

class Cpu {
  private:
    Register pc_;
    Register registers_[kNumberOfRegisters];
    Memory memory_; 
    bool is_finished_;

    Register Fetch();
    InstructionOpcodes Decode(const Register encoded_instr) const;
    InstructionError Execute(const Register instr, const InstructionOpcodes opcode);
    
    // instructions: 
    InstructionError InstructionLui(const Register instr);
    InstructionError InstructionAuipc(const Register instr);
    InstructionError InstructionJal(const Register instr);
    InstructionError InstructionJalr(const Register instr);
    InstructionError InstructionBranchInstr(const Register instr);
    InstructionError InstructionLoadInstr(const Register instr);
    InstructionError InstructionStoreInstr(const Register instr);
    InstructionError InstructionArithmImmInstr(const Register instr);
    InstructionError InstructionArithmRegInstr(const Register instr);
    InstructionError InstructionFenceInstr(const Register instr);
    InstructionError InstructionSystemInstr(const Register instr);
 
  public:
    Cpu(const ELFIO::elfio& elf);
    ~Cpu() = default;

    Register GetPc() const;
    void SetPc(const Register new_pc);

    Register GetRegisterValue(const size_t register_id) const;
    void SetRegisterValue(const size_t register_id, const Register new_value);

    bool GetIsFinished() const;
    void SetIsFinished(const bool is_finished);

    // Register ReadFromMemory(const Address address) const;
    // void WriteToMemory(const Register data, const Address address);

    void Dump() const;
    
    void ExecuteBin();
};

#endif // CPU_HPP_
