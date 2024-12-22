#ifndef CPU_HPP_
#define CPU_HPP_

#include <cstdint>
#include <cstdlib>

#include "sim_cfg.hpp"
#include "cpu_defs.hpp"
#include "decode.hpp"
#include "instructions.hpp"
#include "imemory.hpp"

namespace sim {

class Cpu {
  private:
    Register pc_;
    Register registers_[kNumberOfRegisters];
    bool is_finished_;

    IMemory* memory_;

    InstructionError SyscallHandler();
  public:
    void Init(size_t entry_point, IMemory* memrory);
    ~Cpu() = default;

    Register GetPc() const;
    void SetPc(const Register new_pc);

    Register GetRegisterValue(const size_t register_id) const;
    void SetRegisterValue(const size_t register_id, const Register new_value);

    bool GetIsFinished() const;
    void SetIsFinished(const bool is_finished);

    void Dump() const;
    
    InstructionError Execute(DecodedInstr dec_instr);
};

}


#endif // CPU_HPP_
