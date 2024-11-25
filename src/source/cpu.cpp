#include "cpu.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <climits>

#include "elfio/elfio.hpp"

#include "logger.hpp"

#include "cpu_defs.hpp"
#include "cpu_memory.hpp"
#include "instructions.hpp"

// static ---------------------------------------------------------------------

static RTypeInstr GetRTypeInstr(const Register instr);
static ITypeInstr GetITypeInstr(const Register instr);
static STypeInstr GetSTypeInstr(const Register instr);
static BTypeInstr GetBTypeInstr(const Register instr);
static UTypeInstr GetUTypeInstr(const Register instr);
static JTypeInstr GetJTypeInstr(const Register instr);

static IRegister SignExtension(const Register uvalue, const size_t starting_bit);
static IRegister RegToIReg(const Register uvalue);
static Register IRegToReg(const IRegister value);
static Register ArithmRightShift(const Register value, const size_t shift);

static const char* OpcodeToInstrMnemotic(InstructionOpcodes instr_opcode);

// Cpu private ----------------------------------------------------------------

Register Cpu::Fetch() {
    LogFunctionEntry();

    if (pc_ == 0xDED) {
        is_finished_ = true;
        return 0;
    }

    if (pc_ >= kMemorySize) {
        is_finished_ = true;
        return 0;
    }

    // NOTE if pc_ % 4 != 0?

    Register read_mem = memory_.ReadFromMemory32b(pc_);
    LogVariable("%u", read_mem);

    return read_mem;
}

InstructionOpcodes Cpu::Decode(const Register encoded_instr) const {
    LogFunctionEntry();

    InstructionOpcodes opcode_instr_value = static_cast<InstructionOpcodes>(encoded_instr & kOpcodeMask); 

    LogVariable("%s", OpcodeToInstrMnemotic(opcode_instr_value));

    return opcode_instr_value;
}

InstructionError Cpu::Execute(const Register instr, const InstructionOpcodes opcode) {
    LogFunctionEntry();

    LogVariable("0x%x", instr);
    LogVariable("0x%x", static_cast<Register>(opcode));

    switch (opcode) {
        case InstructionOpcodes::kLui:            return InstructionLui(instr); // +
        case InstructionOpcodes::kAuipc:          return InstructionAuipc(instr); // +
        case InstructionOpcodes::kJal:            return InstructionJal(instr); // +
        case InstructionOpcodes::kJalr:           return InstructionJalr(instr); // +
        case InstructionOpcodes::kBranchInstr:    return InstructionBranchInstr(instr); // +
        case InstructionOpcodes::kLoadInstr:      return InstructionLoadInstr(instr); // +
        case InstructionOpcodes::kStoreInstr:     return InstructionStoreInstr(instr); // +
        case InstructionOpcodes::kArithmImmInstr: return InstructionArithmImmInstr(instr); // +
        case InstructionOpcodes::kArithmRegInstr: return InstructionArithmRegInstr(instr); 
        case InstructionOpcodes::kFenceInstr:     return InstructionFenceInstr(instr);
        case InstructionOpcodes::kSystemInstr:    return InstructionSystemInstr(instr);

        default:
            Log("unknown instrucion\n");
            return InstructionError::kUnknownInstruction;
    }
}

// Cpu public -----------------------------------------------------------------

Cpu::Cpu(const ELFIO::elfio& elf) 
    : memory_(elf)
{
    LogFunctionEntry();

    memset(registers_, 0, sizeof(registers_));
    const size_t kStackPointerIndex = 2;
    registers_[kStackPointerIndex] = memory_.GetMemorySize() - sizeof(Register);

    const size_t kRetAddrIndex = 1;
    registers_[kRetAddrIndex] = 0xDED;
 
    pc_ = elf.get_entry();
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

    if (register_id == 0) {
        return 0;
    }

    return registers_[register_id];
}

void Cpu::SetRegisterValue(const size_t register_id, const Register new_value) {
    assert(register_id < kNumberOfRegisters);

    LogFunctionEntry();

    if (register_id == 0) { // NOTE mb use constant for x0
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

// Register Cpu::ReadFromMemory(const Address address) const {
//     LogFunctionEntry();

//     return memory_.ReadFromMemory(address);
// }

// void Cpu::WriteToMemory(const Register data, const Address address) {
//     LogFunctionEntry();

//     memory_.WriteToMemory(data, address);
// }

void Cpu::Dump() const {
    LogFunctionEntry();

    Log("\n\n");

    LogVariable("0x%x", pc_);

    for (size_t i = 0; i < kNumberOfRegisters; i++) {
        Log("Register x%lu: 0x%x\n", i, registers_[i]);
    } 

    Log("Has finished: %s\n", is_finished_ ? "true" : "false");

    Log("pc dump:\n");
    memory_.Dump(pc_ - 160, pc_ + 160);

    Log("stack pointer dump:\n");
    memory_.Dump(GetRegisterValue(2) - 48, memory_.GetMemorySize()); // FIXME

    Log("\n\n");
}

void Cpu::ExecuteBin() {
    LogFunctionEntry();
 
    size_t exec_end = memory_.GetExecutableEnd();

    while (!is_finished_ && pc_ < exec_end) {
        Register instr = Fetch();
        InstructionOpcodes instruction = Decode(instr);
        InstructionError instr_error = Execute(instr, instruction);
        LogVariable("%u", static_cast<Register>(instr_error));

        Dump(); // FIXME
    }
}

// instructions ---------------------------------------------------------------

InstructionError Cpu::InstructionLui(const Register instr) {
    LogFunctionEntry();

    UTypeInstr u_type_instr = GetUTypeInstr(instr);
    Register result = static_cast<Register>(u_type_instr.imm) << 12u; // read docs
    // NOTE

    SetRegisterValue(u_type_instr.rd, result);
    
    pc_ += sizeof(Register);

    return InstructionError::kOk;
}

InstructionError Cpu::InstructionAuipc(const Register instr) {
    LogFunctionEntry();

    UTypeInstr u_type_instr = GetUTypeInstr(instr);
    Register result = static_cast<Register>(u_type_instr.imm) << 12; // read docs
    // NOTE

    SetRegisterValue(u_type_instr.rd, result + pc_);
    
    pc_ += sizeof(Register);

    return InstructionError::kOk;
}

InstructionError Cpu::InstructionJal(const Register instr) {
    LogFunctionEntry();

    JTypeInstr j_type_instr = GetJTypeInstr(instr);
   
                                            // next instruction
    SetRegisterValue(j_type_instr.rd, pc_ + sizeof(Register));

                // multiples of 2 bytes
    Register offset = (static_cast<Register>(j_type_instr.imm_10_1 << 1)
                      + (static_cast<Register>(j_type_instr.imm_11 << 11)) 
                      + (static_cast<Register>(j_type_instr.imm_19_12 << 12)) 
                      + (static_cast<Register>(j_type_instr.imm_20 << 20)));

    if (offset % sizeof(Register)) { return InstructionError::kMisalignedAddress; }

    const size_t imm_size_bit = 20; // bit
    pc_ += IRegToReg(SignExtension(offset, imm_size_bit - 1));

    return InstructionError::kOk;
}

InstructionError Cpu::InstructionJalr(const Register instr) {
    LogFunctionEntry();

    ITypeInstr i_type_instr = GetITypeInstr(instr);

    SetRegisterValue(i_type_instr.rd, pc_ + sizeof(Register));

    const size_t imm_size_bit = 12; // bit
    pc_ = GetRegisterValue(i_type_instr.rs1) 
          + (SignExtension(i_type_instr.imm, imm_size_bit - 1ull) & (~1u)); // NOTE md +=?

    return InstructionError::kOk;
}

InstructionError Cpu::InstructionBranchInstr(const Register instr) {
    LogFunctionEntry();

    BTypeInstr b_type_instr = GetBTypeInstr(instr);

    bool condition = false;

    Register rs1_value = GetRegisterValue(b_type_instr.rs1);
    Register rs2_value = GetRegisterValue(b_type_instr.rs2);

    IRegister rs1_ivalue = RegToIReg(rs1_value);
    IRegister rs2_ivalue = RegToIReg(rs2_value);

    switch (static_cast<BranchInstruction>(b_type_instr.funct3)) {
        case BranchInstruction::kBeq:  { condition = (rs1_value == rs2_value);   } break;
        case BranchInstruction::kBne:  { condition = (rs1_value != rs2_value);   } break;
        case BranchInstruction::kBlt:  { condition = (rs1_ivalue < rs2_ivalue);  } break; 
        case BranchInstruction::kBge:  { condition = (rs1_ivalue >= rs2_ivalue); } break;
        case BranchInstruction::kBltu: { condition = (rs1_value < rs2_value);    } break;
        case BranchInstruction::kBgeu: { condition = (rs1_value >= rs2_value);   } break;

        default:
            Log("Unknown instruction\n");
            return InstructionError::kUnknownInstruction;
    }

    if (!condition) { 
        Log("Branch not taken\n"); 
        pc_ += sizeof(Register);

        return InstructionError::kOk; 
    }
    Log("Branch taken\n");

    Register offset = (static_cast<Register>(b_type_instr.imm_4_1) << 1) 
                      + (static_cast<Register>(b_type_instr.imm_11) << 11) 
                      + (static_cast<Register>(b_type_instr.imm_12) << 12) 
                      + (static_cast<Register>(b_type_instr.imm_10_5) << 5);
    
    if (offset % sizeof(Register)) {
        return InstructionError::kUnknownInstruction;
    }

    const size_t imm_size_bit = 12; // bit
    Register pc_offset = IRegToReg(SignExtension(offset, imm_size_bit - 1));
    LogVariable("%u", pc_offset);
    pc_ += pc_offset; // NOTE i dont know why +4
    return InstructionError::kOk;
}

InstructionError Cpu::InstructionLoadInstr(const Register instr) {
    LogFunctionEntry();
    
    ITypeInstr i_type_instr = GetITypeInstr(instr);

    const size_t imm_size_bit = 12; // bit
    Address address = GetRegisterValue(i_type_instr.rs1) + SignExtension(i_type_instr.imm, imm_size_bit - 1);

    Register loaded_value = 0;

    switch (static_cast<LoadInstruction>(i_type_instr.funct3)) {
        case LoadInstruction::kLb: { 
            const size_t mem_data_size = 8; // bit
            loaded_value = IRegToReg(SignExtension(memory_.ReadFromMemory8b(address), mem_data_size - 1)); 
        }
        break;
        
        case LoadInstruction::kLh: { 
            const size_t mem_data_size = 16;
            loaded_value = IRegToReg(SignExtension(memory_.ReadFromMemory16b(address), mem_data_size - 1)); 
        }
        break;
        
        case LoadInstruction::kLw: { 
            const size_t mem_data_size = 32;
            loaded_value = IRegToReg(SignExtension(memory_.ReadFromMemory32b(address), mem_data_size - 1)); 
        }
        break;        
    
        case LoadInstruction::kLbu: {
            loaded_value = memory_.ReadFromMemory8b(address);
        }
        break;

        case LoadInstruction::kLhu: {
            loaded_value = memory_.ReadFromMemory16b(address);
        }
        break;

        default:
            Log("Unknown instrucion\n");
            return InstructionError::kUnknownInstruction;
    }

    SetRegisterValue(i_type_instr.rd, loaded_value);

    pc_ += sizeof(Register);

    return InstructionError::kOk;
}

InstructionError Cpu::InstructionStoreInstr(const Register instr) {
    LogFunctionEntry();

    STypeInstr s_type_instr = GetSTypeInstr(instr);

    Register value_to_store = GetRegisterValue(s_type_instr.rs2);

    Register offset = s_type_instr.imm_4_0 + (s_type_instr.imm_11_5 << 5u);

    const size_t imm_size_bit = 12; // bit
    Address address = GetRegisterValue(s_type_instr.rs1) + SignExtension(offset, imm_size_bit - 1);

    switch (static_cast<StoreInstruction>(s_type_instr.funct3)) {
        case StoreInstruction::kSb: {
            memory_.WriteToMemory8b(static_cast<uint8_t>(value_to_store), address);
        }
        break;
        
        case StoreInstruction::kSh: {
            memory_.WriteToMemory16b(static_cast<uint16_t>(value_to_store), address);
        }
        break;

        case StoreInstruction::kSw: {
            memory_.WriteToMemory32b(static_cast<uint32_t>(value_to_store), address);
        }
        break;

        default:
            Log("Unknown instrucion");
            return InstructionError::kUnknownInstruction;
    }

    pc_ += sizeof(Register);

    return InstructionError::kOk;
}

InstructionError Cpu::InstructionArithmImmInstr(const Register instr) {
    LogFunctionEntry();

    ITypeInstr i_type_instr = GetITypeInstr(instr);

    Register result = 0;
    
    const size_t imm_size_bit = 12; // bit
    Register imm = IRegToReg(SignExtension(i_type_instr.imm, imm_size_bit - 1));
    Register reg_value = GetRegisterValue(i_type_instr.rs1);

    switch (static_cast<ArithmImmInstruction>(i_type_instr.funct3)) {
        case ArithmImmInstruction::kAddi:
            result = imm + reg_value;
            break;    

        case ArithmImmInstruction::kSlti:
            result = RegToIReg(reg_value) < RegToIReg(imm) ? 1 : 0;
            break;

        case ArithmImmInstruction::kSltiu:
            result = reg_value < imm ? 1 : 0;
            break;

        case ArithmImmInstruction::kXori:
            result = reg_value ^ imm;
            break;

        case ArithmImmInstruction::kOri:
            result = reg_value | imm;
            break;

        case ArithmImmInstruction::kAndi:
            result = reg_value & imm;
            break;

        case ArithmImmInstruction::kSlli: {
            const size_t shmat_width = 5;
            if ((i_type_instr.imm >> shmat_width) != 0) { return InstructionError::kUnknownInstruction; }

            const Register shmat_mask = 0b1'1111;
            Register shmat = imm & shmat_mask;

            result = reg_value << shmat;
        }
        break;

        case ArithmImmInstruction::kSraiSrli: {
            const size_t shmat_width = 5;
            ArithmImmShiftRight shift_type = static_cast<ArithmImmShiftRight>(imm >> shmat_width);
                
            const Register shmat_mask = 0b1'1111;
            Register shmat = imm & shmat_mask;

            switch (shift_type) {
                case ArithmImmShiftRight::kArithm:
                    result = reg_value >> shmat;
                    break;
                case ArithmImmShiftRight::kLogical:
                    result = ArithmRightShift(reg_value, shmat);
                    break;
                default:
                    Log("Unknown instrucion\n");
                    assert("unknown instruction"); 
                    return InstructionError::kUnknownInstruction;
            }
        }
        break;
            
        default:
            Log("Unknown instrucion\n");
            return InstructionError::kUnknownInstruction;
    }

    SetRegisterValue(i_type_instr.rd, result);

    pc_ += sizeof(Register);

    return InstructionError::kOk;
}

InstructionError Cpu::InstructionArithmRegInstr(const Register instr) {
    LogFunctionEntry();

    RTypeInstr r_type_instr = GetRTypeInstr(instr);

    Register rs1_value = GetRegisterValue(r_type_instr.rs1);
    Register rs2_value = GetRegisterValue(r_type_instr.rs2);

    Register result = 0;

    const size_t shift_mask_width = 5;
    const Register shift_mask = 0b1'1111;

    ArithmRegInstructionSpecial funct7_arithm = static_cast<ArithmRegInstructionSpecial>(r_type_instr.funct7);

    switch (static_cast<ArithmRegInstruction>(r_type_instr.funct3)) {
        case ArithmRegInstruction::kAddSub: {
            switch (funct7_arithm) {
                case ArithmRegInstructionSpecial::kAdd:
                    result = rs1_value + rs2_value;
                    break;
                case ArithmRegInstructionSpecial::kSub:
                    result = rs1_value - rs2_value;
                    break;
                default:
                    Log("Unknown instrucion\n");
                    return InstructionError::kUnknownInstruction;
            }
        }
        break;

        case ArithmRegInstruction::kSll:
            result = rs1_value << (rs2_value & shift_mask);
            break;

        case ArithmRegInstruction::kSlt:
            result = RegToIReg(rs1_value) < RegToIReg(rs2_value) ? 1 : 0; 
            break;

        case ArithmRegInstruction::kSltu:
            result = rs1_value < rs2_value ? 1 : 0;
            break;

        case ArithmRegInstruction::kXor:
            result = rs1_value ^ rs2_value;
            break;

        case ArithmRegInstruction::kSrlSra: {
            switch (static_cast<ArithmImmShiftRight>(funct7_arithm)) {
                case ArithmImmShiftRight::kArithm:
                    result = ArithmRightShift(rs1_value, rs2_value & shift_mask);
                    break;
                case ArithmImmShiftRight::kLogical:
                    result = rs1_value >> (rs2_value & shift_mask);
                    break;
                default:
                    Log("unknown instrucion\n");
                    return InstructionError::kUnknownInstruction;
            }
        }
        break;

        case ArithmRegInstruction::kOr:
            result = rs1_value | rs2_value;
            break;

        case ArithmRegInstruction::kAnd:            
            result = rs1_value & rs2_value;
            break;

        default:
            Log("Unknown instrucion value\n");
            return InstructionError::kUnknownInstruction;      
    }

    SetRegisterValue(r_type_instr.rd, result);

    pc_ += sizeof(Register);

    return InstructionError::kOk;
}

InstructionError Cpu::InstructionFenceInstr(const Register instr) {
    LogFunctionEntry();

    assert(0 && "Unimplemented");

    return InstructionError::kOk;
}

InstructionError Cpu::InstructionSystemInstr(const Register instr) {
    LogFunctionEntry();

    ITypeInstr i_type_instr = GetITypeInstr(instr);

    Log("Fuck my ass\n");

    SetIsFinished(true);

    return InstructionError::kOk;
}

// static ---------------------------------------------------------------------

static RTypeInstr GetRTypeInstr(const Register instr) {
    // LogFunctionEntry();

    RTypeInstr r_type_instr = {};
    std::memcpy(&r_type_instr, &instr, sizeof(instr));

    // FIXME should be %x
    LogVariable("%d", r_type_instr.opcode);
    LogVariable("%d", r_type_instr.rd);
    LogVariable("%d", r_type_instr.funct3);
    LogVariable("%d", r_type_instr.rs1);
    LogVariable("%d", r_type_instr.rs2);
    LogVariable("%d", r_type_instr.funct7);

    return r_type_instr;
}

static ITypeInstr GetITypeInstr(const Register instr) {
    // LogFunctionEntry();

    ITypeInstr i_type_instr = {};
    std::memcpy(&i_type_instr, &instr, sizeof(instr));

    LogVariable("%d", i_type_instr.opcode);
    LogVariable("%d", i_type_instr.rd);
    LogVariable("%d", i_type_instr.funct3);
    LogVariable("%d", i_type_instr.rs1);
    LogVariable("%d", i_type_instr.imm);

    return i_type_instr;
}

static STypeInstr GetSTypeInstr(const Register instr) {
    // LogFunctionEntry();

    STypeInstr s_type_instr = {};
    std::memcpy(&s_type_instr, &instr, sizeof(instr));

    LogVariable("%d", s_type_instr.opcode);
    LogVariable("%d", s_type_instr.imm_4_0);
    LogVariable("%d", s_type_instr.funct3);
    LogVariable("%d", s_type_instr.rs1);
    LogVariable("%d", s_type_instr.rs2);
    LogVariable("%d", s_type_instr.imm_11_5);

    return s_type_instr;
}

static BTypeInstr GetBTypeInstr(const Register instr) {
    // LogFunctionEntry();

    BTypeInstr b_type_instr = {};
    std::memcpy(&b_type_instr, &instr, sizeof(instr));

    LogVariable("%d", b_type_instr.opcode);
    LogVariable("%d", b_type_instr.imm_11);
    LogVariable("%d", b_type_instr.imm_4_1);
    LogVariable("%d", b_type_instr.funct3);
    LogVariable("%d", b_type_instr.rs1);
    LogVariable("%d", b_type_instr.rs2);
    LogVariable("%d", b_type_instr.imm_10_5);
    LogVariable("%d", b_type_instr.imm_12);

    return b_type_instr;
}

static UTypeInstr GetUTypeInstr(const Register instr) {
    // LogFunctionEntry();

    UTypeInstr u_type_instr = {};
    std::memcpy(&u_type_instr, &instr, sizeof(instr));

    LogVariable("%d", u_type_instr.opcode);
    LogVariable("%d", u_type_instr.rd);
    LogVariable("%d", u_type_instr.imm);

    return u_type_instr;
}

static JTypeInstr GetJTypeInstr(const Register instr) {
    // LogFunctionEntry();

    JTypeInstr j_type_instr = {};
    std::memcpy(&j_type_instr, &instr, sizeof(instr));

    LogVariable("%d", j_type_instr.opcode);
    LogVariable("%d", j_type_instr.rd);
    LogVariable("%d", j_type_instr.imm_19_12);
    LogVariable("%d", j_type_instr.imm_11);
    LogVariable("%d", j_type_instr.imm_10_1);
    LogVariable("%d", j_type_instr.imm_20);

    return j_type_instr;
}

static IRegister SignExtension(const Register uvalue, const size_t starting_bit) {
    // LogFunctionEntry();
    
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

    LogVariable("0x%x", static_cast<uint32_t>(value));

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
