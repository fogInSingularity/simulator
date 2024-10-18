#ifndef INSTRUCTION_SEMANTIC_HPP_
#define INSTRUCTION_SEMANTIC_HPP_

#include "cpu.hpp"
#include "instructions.hpp"

InstructionError InstructionLui(Cpu* const cpu, const Register instr);
InstructionError InstructionAuipc(Cpu* const cpu, const Register instr);
InstructionError InstructionJal(Cpu* const cpu, const Register instr);
InstructionError InstructionJalr(Cpu* const cpu, const Register instr);
InstructionError InstructionBranchInstr(Cpu* const cpu, const Register instr);
InstructionError InstructionLoadInstr(Cpu* const cpu, const Register instr);
InstructionError InstructionStoreInstr(Cpu* const cpu, const Register instr);
InstructionError InstructionArithmImmInstr(Cpu* const cpu, const Register instr);
InstructionError InstructionArithmRegInstr(Cpu* const cpu, const Register instr);
InstructionError InstructionFenceInstr(Cpu* const cpu, const Register instr);
InstructionError InstructionSystemInstr(Cpu* const cpu, const Register instr);

#endif // INSTRUCTION_SEMANTIC_HPP_
