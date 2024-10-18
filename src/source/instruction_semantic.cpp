#include "instruction_semantic.hpp"

#include <cassert>
#include <climits>
#include <cstring>

#include "cpu_defs.hpp"
#include "instructions.hpp"
#include "cpu.hpp"
#include "logging.h"

// static ---------------------------------------------------------------------

static RTypeInstr GetRTypeInstr(const Register instr);
static ITypeInstr GetITypeInstr(const Register instr);

static IRegister SignExtension(const Register uvalue, const size_t starting_bit);
static IRegister RegToIReg(const Register uvalue);
static Register IRegToReg(const IRegister value);

static InstructionError InstructionArithmImmAddi(Cpu* const cpu, const ITypeInstr i_type_instr);
static InstructionError InstructionArithmImmSlti(Cpu* const cpu, const ITypeInstr i_type_instr);
static InstructionError InstructionArithmImmSltiu(Cpu* const cpu, const ITypeInstr i_type_instr);
static InstructionError InstructionArithmImmXori(Cpu* const cpu, const ITypeInstr i_type_instr);
static InstructionError InstructionArithmImmOri(Cpu* const cpu, const ITypeInstr i_type_instr);
static InstructionError InstructionArithmImmAndi(Cpu* const cpu, const ITypeInstr i_type_instr);

static InstructionError InstructionArithmRegAddSub(Cpu* const cpu, const RTypeInstr r_type_instr);

// global ---------------------------------------------------------------------

InstructionError InstructionLui(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    assert(0 && "Unimplemented");
}

InstructionError InstructionAuipc(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    assert(0 && "Unimplemented");
}

InstructionError InstructionJal(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    assert(0 && "Unimplemented");
}

InstructionError InstructionJalr(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    assert(0 && "Unimplemented");
}

InstructionError InstructionBranchInstr(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    assert(0 && "unimplemented");
}

InstructionError InstructionLoadInstr(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    assert(0 && "Unimplemented");
}

InstructionError InstructionStoreInstr(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    assert(0 && "Unimplemented");
}

InstructionError InstructionArithmImmInstr(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    ITypeInstr i_type_instr = GetITypeInstr(instr);

    switch (static_cast<ArithmImmInstruction>(i_type_instr.funct3)) {
        case ArithmImmInstruction::kAddi:  return InstructionArithmImmAddi(cpu, i_type_instr);
        case ArithmImmInstruction::kSlti:  return InstructionArithmImmSlti(cpu, i_type_instr);
        case ArithmImmInstruction::kSltiu: return InstructionArithmImmSltiu(cpu, i_type_instr);
        case ArithmImmInstruction::kXori:  return InstructionArithmImmXori(cpu, i_type_instr);
        case ArithmImmInstruction::kOri:   return InstructionArithmImmOri(cpu, i_type_instr);
        case ArithmImmInstruction::kAndi:  return InstructionArithmImmAndi(cpu, i_type_instr);

        default:
            Log("Unknown funct3 value\n");
            return InstructionError::kUnknownInstruction;
    }

    assert(0 && "shouldnt fallthrow switch");
}

InstructionError InstructionArithmRegInstr(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    RTypeInstr r_type_instr = GetRTypeInstr(instr);
    switch (static_cast<ArithmRegInstruction>(r_type_instr.funct3)) {
        case ArithmRegInstruction::kAddSub: return InstructionArithmRegAddSub(cpu, r_type_instr);
    
        default:
            Log("Unknown funct3 value\n");
            return InstructionError::kUnknownInstruction;      
    }
    assert(0 && "Unimplemented");
}

InstructionError InstructionFenceInstr(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    assert(0 && "Unimplemented");
}

InstructionError InstructionSystemInstr(Cpu* const cpu, const Register instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    assert(0 && "Unimplemented");
}

// static ---------------------------------------------------------------------

static RTypeInstr GetRTypeInstr(const Register instr) {
    LogFunctionEntry();

    RTypeInstr r_type_instr = {};
    std::memcpy(&r_type_instr, &instr, sizeof(instr));

    LogVariable("%x", r_type_instr.opcode);
    LogVariable("%x", r_type_instr.rd);
    LogVariable("%x", r_type_instr.funct3);
    LogVariable("%x", r_type_instr.rs1);
    LogVariable("%x", r_type_instr.rs2);
    LogVariable("%x", r_type_instr.funct7);

    return r_type_instr;
}

static ITypeInstr GetITypeInstr(const Register instr) {
    LogFunctionEntry();

    ITypeInstr i_type_instr = {};
    std::memcpy(&i_type_instr, &instr, sizeof(instr));

    LogVariable("%x", i_type_instr.opcode);
    LogVariable("%x", i_type_instr.rd);
    LogVariable("%x", i_type_instr.funct3);
    LogVariable("%x", i_type_instr.rs1);
    LogVariable("%x", i_type_instr.imm);

    return i_type_instr;
}

static IRegister SignExtension(const Register uvalue, const size_t starting_bit) {
    LogFunctionEntry();
    
    static_assert(sizeof(Register) == sizeof(IRegister), "Register and IRegister have different sizes");
    IRegister value = RegToIReg(uvalue);

    const Register signed_bit_mask = 1 << starting_bit;
    if (!(uvalue & signed_bit_mask)) {
        return value;
    }

    Register move_bit = 1 << (starting_bit + 1);
    for (size_t i = starting_bit + 1; i < sizeof(uvalue) * CHAR_BIT; i++) {
        value |= move_bit;
        move_bit <<= 1;
    }

    LogVariable("%x", value);

    return value;
}

static IRegister RegToIReg(const Register uvalue) {
    LogFunctionEntry();

    IRegister value = 0;

    static_assert(sizeof(value) == sizeof(uvalue), "size of uvalue and value differ");
    std::memcpy(&value, &uvalue, sizeof(uvalue));

    return value;
}

static Register IRegToReg(const IRegister value) {
    LogFunctionEntry();

    Register uvalue = 0;

    static_assert(sizeof(value) == sizeof(uvalue), "size of uvalue and value differ");
    std::memcpy(&uvalue, &value, sizeof(value));

    return uvalue;
}

// --------------------

static InstructionError InstructionArithmImmAddi(Cpu* const cpu, const ITypeInstr i_type_instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    const size_t imm_size_bit = 12; // bit
    IRegister imm = SignExtension(i_type_instr.imm, imm_size_bit - 1);
    IRegister reg_value = RegToIReg(cpu->GetRegisterValue(i_type_instr.rs1));

    Register uresult = IRegToReg(reg_value + imm);

    cpu->SetRegisterValue(i_type_instr.rd, uresult);

    Register pc = cpu->GetPc();
    pc += sizeof(Register);
    cpu->SetPc(pc);
 
    return InstructionError::kOk;
}

static InstructionError InstructionArithmImmSlti(Cpu* const cpu, const ITypeInstr i_type_instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    const size_t imm_size_bit = 12; // bit
    IRegister imm = SignExtension(i_type_instr.imm, imm_size_bit - 1);
    IRegister reg_value = RegToIReg(cpu->GetRegisterValue(i_type_instr.rs1));

    Register uresult = reg_value < imm ? 1 : 0;

    cpu->SetRegisterValue(i_type_instr.rd, uresult);

    Register pc = cpu->GetPc();
    pc += sizeof(Register);
    cpu->SetPc(pc);

    return InstructionError::kOk;
}

static InstructionError InstructionArithmImmSltiu(Cpu* const cpu, const ITypeInstr i_type_instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    const size_t imm_size_bit = 12; // bit
    Register uimm = IRegToReg(SignExtension(i_type_instr.imm, imm_size_bit - 1));
    Register ureg_value = cpu->GetRegisterValue(i_type_instr.rs1);

    Register uresult = ureg_value < uimm ? 1 : 0;

    cpu->SetRegisterValue(i_type_instr.rd, uresult);

    Register pc = cpu->GetPc();
    pc += sizeof(Register);
    cpu->SetPc(pc);

    return InstructionError::kOk;
}

static InstructionError InstructionArithmImmXori(Cpu* const cpu, const ITypeInstr i_type_instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    const size_t imm_size_bit = 12; // bit
    IRegister imm = SignExtension(i_type_instr.imm, imm_size_bit - 1);
    IRegister reg_value = RegToIReg(cpu->GetRegisterValue(i_type_instr.rs1));

    Register uresult = IRegToReg(reg_value ^ imm);

    cpu->SetRegisterValue(i_type_instr.rd, uresult);

    Register pc = cpu->GetPc();
    pc += sizeof(Register);
    cpu->SetPc(pc);

    return InstructionError::kOk;
}

static InstructionError InstructionArithmImmOri(Cpu* const cpu, const ITypeInstr i_type_instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    const size_t imm_size_bit = 12; // bit
    IRegister imm = SignExtension(i_type_instr.imm, imm_size_bit - 1);
    IRegister reg_value = RegToIReg(cpu->GetRegisterValue(i_type_instr.rs1));

    Register uresult = IRegToReg(reg_value | imm);

    cpu->SetRegisterValue(i_type_instr.rd, uresult);

    Register pc = cpu->GetPc();
    pc += sizeof(Register);
    cpu->SetPc(pc);
 
    return InstructionError::kOk;
}

static InstructionError InstructionArithmImmAndi(Cpu* const cpu, const ITypeInstr i_type_instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    const size_t imm_size_bit = 12; // bit
    IRegister imm = SignExtension(i_type_instr.imm, imm_size_bit - 1);
    IRegister reg_value = RegToIReg(cpu->GetRegisterValue(i_type_instr.rs1));

    Register uresult = IRegToReg(reg_value & imm);

    cpu->SetRegisterValue(i_type_instr.rd, uresult);

    Register pc = cpu->GetPc();
    pc += sizeof(Register);
    cpu->SetPc(pc);
 
    return InstructionError::kOk;
}

// ----------------------

static InstructionError InstructionArithmRegAddSub(Cpu* const cpu, const RTypeInstr r_type_instr) {
    assert(cpu != nullptr);

    LogFunctionEntry();

    IRegister reg1 = 0;
    Register ureg1 = cpu->GetRegisterValue(r_type_instr.rs1);
    std::memcpy(&reg1, &ureg1, sizeof(ureg1)); 

    IRegister reg2 = 0;
    Register ureg2 = cpu->GetRegisterValue(r_type_instr.rs2);
    std::memcpy(&reg2, &ureg2, sizeof(ureg2));

    IRegister result = 0;
    switch (static_cast<ArithmRegInstructionSpecial>(r_type_instr.funct7)) {
        case ArithmRegInstructionSpecial::kAdd:
            result = reg1 + reg2;
            break;
        case ArithmRegInstructionSpecial::kSub:
            result = reg1 - reg2;
            break;
        
        default:
            Log("Unknown funct7 value\n");
            return InstructionError::kUnknownInstruction;
    }

    Register u_result = 0;
    std::memcpy(&u_result, &result, sizeof(u_result));

    cpu->SetRegisterValue(r_type_instr.rd, u_result);

    Register pc = cpu->GetPc();
    pc += sizeof(Register);
    cpu->SetPc(pc);

    return InstructionError::kOk;
}


