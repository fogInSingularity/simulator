#include "cpu.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <climits>

#include <unistd.h>

#include "log_helper.hpp"

#include "imemory.hpp"

#include "cpu_defs.hpp"
#include "instructions.hpp"
#include "sim_cfg.hpp"
#include "spdlog/spdlog.h"

namespace sim {

// static ---------------------------------------------------------------------

static IRegister SignExtension(const Register uvalue, const size_t starting_bit);
static IRegister RegToIReg(const Register uvalue);
static Register IRegToReg(const IRegister value);
static Register ArithmRightShift(const Register value, const size_t shift);

static const char* OpcodeToInstrMnemotic(InstructionOpcodes instr_opcode);

// Cpu private ----------------------------------------------------------------

InstructionError Cpu::SyscallHandler() {
    LogFunctionEntry();

    spdlog::trace("Syscall number: {}", registers_[RegisterAliases::kArgument7]);
    for (size_t i = 0; i <= 7; i++) {
        spdlog::trace("arg{}: {}", i, registers_[RegisterAliases::kArgument0 + i]);
    }

    Register syscall_id = registers_[kSyscallIdRegister];
    switch (syscall_id) {
        case SyscallIds::kWrite: {
            Register fd_to_write = GetRegisterValue(RegisterAliases::kArgument0);
            uint8_t* buffer = memory_->GetData() + GetRegisterValue(RegisterAliases::kArgument1);
            size_t buffer_size = GetRegisterValue(RegisterAliases::kArgument2);

            LogVar(fd_to_write);
            LogVar((void*)buffer);
            LogVar(buffer_size);

            ssize_t ret_value = write(fd_to_write, buffer, buffer_size);
            SetRegisterValue(RegisterAliases::kArgument0, ret_value);
        }
        break;
        case SyscallIds::kExit: {
            SetIsFinished(true);
        }
        break;
        default:
            spdlog::error("Unknown syscall");
            return InstructionError::kUnknownInstruction;
    }

    return InstructionError::kOk;
}

// Cpu public -----------------------------------------------------------------

void Cpu::Init(size_t entry_point, IMemory* memory) {
    LogFunctionEntry();

    assert(memory != nullptr);

    memory_ = memory;

    memset(registers_, 0, sizeof(registers_));
    registers_[RegisterAliases::kStackPointer] = static_cast<Register>(memory_->GetMemorySize()) - sizeof(Register);
    registers_[RegisterAliases::kRetAddr] = kStartingReturnAddress;
 
    pc_ = static_cast<Register>(entry_point);
    spdlog::debug("Cpu init pc: {:x}({})", pc_, pc_);

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

    LogFunctionEntry();

    if (register_id == RegisterAliases::kMachineZero) {
        return 0;
    }

    return registers_[register_id];
}

void Cpu::SetRegisterValue(const size_t register_id, const Register new_value) {
    assert(register_id < kNumberOfRegisters);

    LogFunctionEntry();

    if (register_id == RegisterAliases::kMachineZero) {
        return ;
    } 

    registers_[register_id] = new_value;
}

bool Cpu::GetIsFinished() const {
    LogFunctionEntry();
    
    return is_finished_;    
}

void Cpu::SetIsFinished(const bool is_finished) {
    LogFunctionEntry();

    is_finished_ = is_finished;
}

void Cpu::Dump() const {
    LogFunctionEntry();
    
    spdlog::info("");
    spdlog::info("Cpu dump:");

    LogVarX(pc_);

    for (size_t i = 0; i < kNumberOfRegisters; i++) {
        if (i == RegisterAliases::kRetAddr) {
            spdlog::info("Register x{}(ret addr): 0x{:x}({})", i, registers_[i], registers_[i]);
        } else if (i == RegisterAliases::kStackPointer) {
            spdlog::info("Register x{}(stack pointer): 0x{:x}({})", i, registers_[i], registers_[i]);
        } else {
            spdlog::info("Register x{}: 0x{:x}", i, registers_[i]);
        }
    } 

    spdlog::info("");

    spdlog::info("Has finished: {}", is_finished_ ? "true" : "false");
}

InstructionError Cpu::Execute(DecodedInstr dec_instr) {
    LogFunctionEntry();

    LogVar(static_cast<Register>(dec_instr.instr_mnem));
    LogVar(pc_);

    InstructionError err = InstructionError::kOk;

    switch (dec_instr.instr_mnem) {
        case InstructionMnemonic::kLui: {
            SetRegisterValue(dec_instr.instr.u_type.rd, dec_instr.instr.u_type.imm << 12u);
            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kAuipc: {
            SetRegisterValue(dec_instr.instr.u_type.rd, (dec_instr.instr.u_type.imm << 12u) + pc_);
            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kJal: {
            SetRegisterValue(dec_instr.instr.j_type.rd, pc_ + sizeof(Register));
            Register pc_offset = IRegToReg(SignExtension(dec_instr.instr.j_type.imm, dec_instr.instr.j_type.imm_size_bit - 1));
            LogVar(pc_offset);
            pc_ += pc_offset;
        }
        break;
        case InstructionMnemonic::kJalr: {
            Register tmp_pc = pc_ + sizeof(Register); 
            Register pc_offset = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            LogVar(pc_offset);
            pc_ = (GetRegisterValue(dec_instr.instr.i_type.rs1) + pc_offset) & (~1);
            SetRegisterValue(dec_instr.instr.i_type.rd, tmp_pc);
        }
        break;
        case InstructionMnemonic::kBeq: {
            if (!(GetRegisterValue(dec_instr.instr.b_type.rs1) == GetRegisterValue(dec_instr.instr.b_type.rs2))) {
                pc_ += sizeof(Register);
                return InstructionError::kOk;
            }

            pc_ += IRegToReg(SignExtension(dec_instr.instr.b_type.imm, dec_instr.instr.b_type.imm_size_bit - 1));
        }
        break;
        case InstructionMnemonic::kBne: {
            if (!(GetRegisterValue(dec_instr.instr.b_type.rs1) != GetRegisterValue(dec_instr.instr.b_type.rs2))) {
                pc_ += sizeof(Register);
                return InstructionError::kOk;
            }

            pc_ += IRegToReg(SignExtension(dec_instr.instr.b_type.imm, dec_instr.instr.b_type.imm_size_bit - 1));
        }
        break;
        case InstructionMnemonic::kBlt: {
            IRegister rs1_ivalue = RegToIReg(GetRegisterValue(dec_instr.instr.b_type.rs1));
            IRegister rs2_ivalue = RegToIReg(GetRegisterValue(dec_instr.instr.b_type.rs2));

            if (!(rs1_ivalue < rs2_ivalue)) {
                pc_ += sizeof(Register);
                return InstructionError::kOk;
            }

            pc_ += IRegToReg(SignExtension(dec_instr.instr.b_type.imm, dec_instr.instr.b_type.imm_size_bit - 1));
        }
        break;
        case InstructionMnemonic::kBge: {
            IRegister rs1_ivalue = RegToIReg(GetRegisterValue(dec_instr.instr.b_type.rs1));
            IRegister rs2_ivalue = RegToIReg(GetRegisterValue(dec_instr.instr.b_type.rs2));

            if (!(rs1_ivalue >= rs2_ivalue)) {
                pc_ += sizeof(Register);
                return InstructionError::kOk;
            }

            pc_ += IRegToReg(SignExtension(dec_instr.instr.b_type.imm, dec_instr.instr.b_type.imm_size_bit - 1));
        }
        break;
        case InstructionMnemonic::kBltu: {
            if (!(GetRegisterValue(dec_instr.instr.b_type.rs1) < GetRegisterValue(dec_instr.instr.b_type.rs2))) {
                pc_ += sizeof(Register);
                return InstructionError::kOk;
            }

            pc_ += IRegToReg(SignExtension(dec_instr.instr.b_type.imm, dec_instr.instr.b_type.imm_size_bit - 1));
        }
        break;
        case InstructionMnemonic::kBgeu: {
            if (!(GetRegisterValue(dec_instr.instr.b_type.rs1) >= GetRegisterValue(dec_instr.instr.b_type.rs2))) {
                pc_ += sizeof(Register);
                return InstructionError::kOk;
            }

            pc_ += IRegToReg(SignExtension(dec_instr.instr.b_type.imm, dec_instr.instr.b_type.imm_size_bit - 1));
        }
        break;
        case InstructionMnemonic::kLb: {
            Address address = GetRegisterValue(dec_instr.instr.i_type.rs1) + IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register loaded_value = IRegToReg(SignExtension(memory_->ReadFromMemory8b(address), sizeof(uint8_t) - 1));
            SetRegisterValue(dec_instr.instr.i_type.rd, loaded_value);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kLh: {
            Address address = GetRegisterValue(dec_instr.instr.i_type.rs1) + IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register loaded_value = IRegToReg(SignExtension(memory_->ReadFromMemory16b(address), sizeof(uint16_t) - 1));
            SetRegisterValue(dec_instr.instr.i_type.rd, loaded_value);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kLw: {
            Address address = GetRegisterValue(dec_instr.instr.i_type.rs1) + IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register loaded_value = memory_->ReadFromMemory32b(address);
            SetRegisterValue(dec_instr.instr.i_type.rd, loaded_value);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kLbu: {
            Address address = GetRegisterValue(dec_instr.instr.i_type.rs1) + IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register loaded_value = memory_->ReadFromMemory8b(address);
            SetRegisterValue(dec_instr.instr.i_type.rd, loaded_value);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kLhu: {
            Address address = GetRegisterValue(dec_instr.instr.i_type.rs1) + IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register loaded_value = memory_->ReadFromMemory16b(address);
            SetRegisterValue(dec_instr.instr.i_type.rd, loaded_value);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSb: {
            Register offset = IRegToReg(SignExtension(dec_instr.instr.s_type.imm, dec_instr.instr.s_type.imm_size_bit - 1));
            Address address = GetRegisterValue(dec_instr.instr.s_type.rs1) + offset;
            
            memory_->WriteToMemory8b(GetRegisterValue(dec_instr.instr.s_type.rs2), address);
            
            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSh: {
            Register offset = IRegToReg(SignExtension(dec_instr.instr.s_type.imm, dec_instr.instr.s_type.imm_size_bit - 1));
            Address address = GetRegisterValue(dec_instr.instr.s_type.rs1) + offset;

            memory_->WriteToMemory16b(GetRegisterValue(dec_instr.instr.s_type.rs2), address);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSw: {
            Register offset = IRegToReg(SignExtension(dec_instr.instr.s_type.imm, dec_instr.instr.s_type.imm_size_bit - 1));
            Address address = GetRegisterValue(dec_instr.instr.s_type.rs1) + offset;

            memory_->WriteToMemory32b(GetRegisterValue(dec_instr.instr.s_type.rs2), address);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kAddi: {
            Register imm = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register reg_value = GetRegisterValue(dec_instr.instr.i_type.rs1);
            Register result = reg_value + imm;
            SetRegisterValue(dec_instr.instr.i_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSlti: {
            Register imm = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register reg_value = GetRegisterValue(dec_instr.instr.i_type.rs1);
            Register result = RegToIReg(reg_value) < RegToIReg(imm) ? 1 : 0;
            SetRegisterValue(dec_instr.instr.i_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSltiu: {
            Register imm = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register reg_value = GetRegisterValue(dec_instr.instr.i_type.rs1);
            Register result = reg_value < imm ? 1 : 0;
            SetRegisterValue(dec_instr.instr.i_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kXori: {
            Register imm = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register reg_value = GetRegisterValue(dec_instr.instr.i_type.rs1);
            Register result = reg_value ^ imm;
            SetRegisterValue(dec_instr.instr.i_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kOri: {
            Register imm = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register reg_value = GetRegisterValue(dec_instr.instr.i_type.rs1);
            Register result = reg_value | imm;
            SetRegisterValue(dec_instr.instr.i_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kAndi: {
            Register imm = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register reg_value = GetRegisterValue(dec_instr.instr.i_type.rs1);
            Register result = reg_value & imm;
            SetRegisterValue(dec_instr.instr.i_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSlli: {
            Register imm = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register reg_value = GetRegisterValue(dec_instr.instr.i_type.rs1);
            const Register shmat_mask = 0b1'1111;
            Register result = reg_value << (imm & shmat_mask);
            SetRegisterValue(dec_instr.instr.i_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSrli: {
            Register imm = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register reg_value = GetRegisterValue(dec_instr.instr.i_type.rs1);
            const Register shmat_mask = 0b1'1111;
            Register result = reg_value >> (imm & shmat_mask);
            SetRegisterValue(dec_instr.instr.i_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSrai: {
            Register imm = IRegToReg(SignExtension(dec_instr.instr.i_type.imm, dec_instr.instr.i_type.imm_size_bit - 1));
            Register reg_value = GetRegisterValue(dec_instr.instr.i_type.rs1);
            const Register shmat_mask = 0b1'1111;
            Register result = ArithmRightShift(reg_value, (imm & shmat_mask));
            SetRegisterValue(dec_instr.instr.i_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kAdd: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);

            Register result = rs1_value + rs2_value;
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSub: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);

            Register result = rs1_value - rs2_value;
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSlt: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);

            Register result = RegToIReg(rs1_value) < RegToIReg(rs2_value) ? 1 : 0;
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSltu: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);

            Register result = rs1_value < rs2_value ? 1 : 0;
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kXor: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);

            Register result = rs1_value ^ rs2_value;
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kOr: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);

            Register result = rs1_value | rs2_value;
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kAnd: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);

            Register result = rs1_value & rs2_value;
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSll: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);
            const Register shift_mask = 0b1'1111;

            Register result = rs1_value << (rs2_value & shift_mask);
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSrl: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);
            const Register shift_mask = 0b1'1111;

            Register result = rs1_value >> (rs2_value & shift_mask);
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSra: {
            Register rs1_value = GetRegisterValue(dec_instr.instr.r_type.rs1);
            Register rs2_value = GetRegisterValue(dec_instr.instr.r_type.rs2);
            const Register shift_mask = 0b1'1111;

            Register result = ArithmRightShift(rs1_value, (rs2_value & shift_mask));
            SetRegisterValue(dec_instr.instr.r_type.rd, result);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kFence: {
            assert(0 && "fence is still unsupported");

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kFence_i: {
            assert(0 && "fence is still unsupported");

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kScall: {
            // assert(0 && "syscalls is stil unsupported");

            spdlog::info("Syscall instruction encountered");

            err = SyscallHandler();

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kSbreak: {
            SetIsFinished(true);

            pc_ += sizeof(Register);
        }
        break;
        case InstructionMnemonic::kUnkownMnem:
        default:
            assert(0 && "unknown instruction");
            return InstructionError::kUnknownInstruction;
    }

    return err;
}

// static ---------------------------------------------------------------------

static IRegister SignExtension(const Register uvalue, const size_t starting_bit) {
    LogFunctionEntry();
    LogVar(starting_bit);

    static_assert(sizeof(Register) == sizeof(IRegister), "Register and IRegister have different sizes");

    const Register signed_bit_mask = 1 << starting_bit;
    if (!(uvalue & signed_bit_mask)) {
        return RegToIReg(uvalue);
    }

    Register res = uvalue;
    Register move_bit = 1 << (starting_bit + 1);
    for (size_t i = starting_bit + 1; i < sizeof(uvalue) * CHAR_BIT; i++) {
        res |= move_bit;
        move_bit <<= 1;
    }
    
    IRegister value = RegToIReg(res);

    LogVar(static_cast<uint32_t>(value));

    return value;
}

static IRegister RegToIReg(const Register uvalue) {
    // LogFunctionEntry();

    IRegister value = 0;

    static_assert(sizeof(value) == sizeof(uvalue), "size of uvalue and value differ");
    std::memcpy(&value, &uvalue, sizeof(uvalue));

    return value;
}

static Register IRegToReg(const IRegister value) {
    // LogFunctionEntry();

    Register uvalue = 0;

    static_assert(sizeof(value) == sizeof(uvalue), "size of uvalue and value differ");
    std::memcpy(&uvalue, &value, sizeof(value));

    return uvalue;
}

static Register ArithmRightShift(const Register value, const size_t shift) {
    LogFunctionEntry();

    Register msb = value & static_cast<Register>((1 << (sizeof(Register) * CHAR_BIT - 1)));
    Register new_value = value >> shift;

    for (size_t move_shift = 0; move_shift < shift; move_shift++) {
        new_value |= msb;
        msb >>= 1;
    }

    return new_value;
}

static const char* OpcodeToInstrMnemotic(InstructionOpcodes instr_opcode) {
    // LogFunctionEntry();

    switch (instr_opcode) {
        case InstructionOpcodes::kUnknown:          return TO_STR(InstructionOpcodes::kUnknown);
        case InstructionOpcodes::kLui:              return TO_STR(InstructionOpcodes::kLui);
        case InstructionOpcodes::kAuipc:            return TO_STR(InstructionOpcodes::kAuipc);
        case InstructionOpcodes::kJal:              return TO_STR(InstructionOpcodes::kJal);
        case InstructionOpcodes::kJalr:             return TO_STR(InstructionOpcodes::kJalr);
        case InstructionOpcodes::kBranchInstr:      return TO_STR(InstructionOpcodes::kBranchInstr);
        case InstructionOpcodes::kLoadInstr:        return TO_STR(InstructionOpcodes::kLoadInstr);
        case InstructionOpcodes::kStoreInstr:       return TO_STR(InstructionOpcodes::kStoreInstr);
        case InstructionOpcodes::kArithmImmInstr:   return TO_STR(InstructionOpcodes::kArithmImmInstr);
        case InstructionOpcodes::kArithmRegInstr:   return TO_STR(InstructionOpcodes::kArithmRegInstr);
        case InstructionOpcodes::kFenceInstr:       return TO_STR(InstructionOpcodes::kFenceInstr);
        case InstructionOpcodes::kSystemInstr:      return TO_STR(InstructionOpcodes::kSystemInstr);
        default:
            return "<unknown instruction>";
    }
}

} // namespace sim
