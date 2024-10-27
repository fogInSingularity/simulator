#ifndef INSTRUCTIONS_HPP_
#define INSTRUCTIONS_HPP_

#include "cpu_defs.hpp"

static const Register kOpcodeMask = 0b111'1111;

struct RTypeInstr {
    Register opcode : 7;
    Register rd     : 5;
    Register funct3 : 3;
    Register rs1    : 5;
    Register rs2    : 5;
    Register funct7 : 7;
};

struct ITypeInstr {
    Register opcode : 7;
    Register rd     : 5;
    Register funct3 : 3;
    Register rs1    : 5;
    Register imm    : 12;
};

struct STypeInstr {
    Register opcode   : 7;
    Register imm_4_0  : 5;
    Register funct3   : 3;
    Register rs1      : 5;
    Register rs2      : 5;
    Register imm_11_5 : 7;
};

struct BTypeInstr {
    Register opcode   : 7;
    Register imm_11   : 1;
    Register imm_4_1  : 4;
    Register funct3   : 3;
    Register rs1      : 5;
    Register rs2      : 5;
    Register imm_10_5 : 6;
    Register imm_12   : 1;
};

struct UTypeInstr {
    Register opcode : 7;
    Register rd     : 5;
    Register imm    : 20;
};

struct JTypeInstr {
    Register opcode    : 7;
    Register rd        : 5;
    Register imm_19_12 : 8; 
    Register imm_11    : 1;
    Register imm_10_1  : 10;
    Register imm_20    : 1;
};

// opcodes --------------------------------------------------------------------

enum class InstructionOpcodes : Register {
    kUnknown        = 0b000'0000,
    kLui            = 0b011'0111, // load upper immediate: load 20-bit high bits to register
    kAuipc          = 0b001'0111, // add upper immediate to pc: pc + imm
    kJal            = 0b110'1111, // jump and link
    kJalr           = 0b110'0111, // jump and link register
    kBranchInstr    = 0b110'0011, // branch family of instructions
    kLoadInstr      = 0b000'0011, // load family of isntructions
    kStoreInstr     = 0b010'0011, // store family of instructions    
    kArithmImmInstr = 0b001'0011, // arithmetic operations with immediate foi
    kArithmRegInstr = 0b011'0011, // arithmetic operations with register foi
    kFenceInstr     = 0b000'1111, // fence foi
    kSystemInstr    = 0b111'0011, // system foi   
};

// funct3 ---------------------------------------------------------------------

enum class BranchInstruction : Register {
    kBeq  = 0b000, // branch if equal
    kBne  = 0b001, // branch if not equal
    kBlt  = 0b100, // branch if less than
    kBltu = 0b110, // branch if less than (usigned)
    kBge  = 0b101, // branch if greater or equal than 
    kBgeu = 0b111, // branch if greater or equal than (usigned)
};

enum class LoadInstruction : Register {
    kLb  = 0b000, // load byte
    kLh  = 0b001, // load half
    kLw  = 0b010, // load word
    kLbu = 0b100, // load byte (usigned)
    kLhu = 0b101, // load half (usigned)
};

enum class StoreInstruction : Register {
    kSb = 0b000, // store byte
    kSh = 0b001, // store half
    kSw = 0b010, // store word
};

enum class ArithmImmInstruction : Register {
    kAddi     = 0b000,
    kSlti     = 0b010,
    kSltiu    = 0b011,
    kXori     = 0b100,
    kOri      = 0b110,
    kAndi     = 0b111,
    
    kSlli     = 0b001,
    kSraiSrli = 0b101,
};

enum class ArithmRegInstruction : Register {
    kAddSub = 0b000,
    kSll    = 0b001,
    kSlt    = 0b010,
    kSltu   = 0b011,
    kXor    = 0b100,
    kSrlSra = 0b101,
    kOr     = 0b110,
    kAnd    = 0b111,
};

enum class FenceInstruction : Register {
    kDefault     = 0b000, // FENCE used to control order of io
    kInstruction = 0b001, // FENCE.I sync for data + instructions
};

enum class SystemInstruction : Register {
    // FIXME 
};

// funct7 ---------------------------------------------------------------------

enum class ArithmRegInstructionSpecial : Register {
    kAdd  = 0b000'0000,
    kSub  = 0b010'0000,
    kSll  = 0b000'0000,
    kSlt  = 0b000'0000,
    kSltu = 0b000'0000,
    kXor  = 0b000'0000,
    kSrl  = 0b000'0000,
    kSra  = 0b010'0000,
    kOr   = 0b000'0000,
    kAnd  = 0b000'0000,
};

enum ArithmImmShiftRight : Register {
    kLogical = 0b000'0000,
    kArithm  = 0b010'0000,
};

// Errors ---------------------------------------------------------------------

enum class InstructionError {
    kOk                 = 0,
    kUnknownInstruction = 1,
    kMisalignedAddress  = 2,
};

#endif // INSTRUCTIONS_HPP_
