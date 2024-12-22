#include "decode.hpp"

#include <cstring>
#include <cassert>

#include "log_helper.hpp"

#include "instructions.hpp"
#include "sim_cfg.hpp"
#include "spdlog/spdlog.h"

namespace sim {

// static ---------------------------------------------------------------------

static InstructionMnemonic GetMnemonicFromOpcode(Register opcode);

static RTypeInstr GetRTypeInstr(const Register instr);
static ITypeInstr GetITypeInstr(const Register instr);
static STypeInstr GetSTypeInstr(const Register instr);
static BTypeInstr GetBTypeInstr(const Register instr);
static UTypeInstr GetUTypeInstr(const Register instr);
static JTypeInstr GetJTypeInstr(const Register instr);

// global ---------------------------------------------------------------------

DecodedInstr Decode(Register enc_instr) {
    InstructionOpcodes opcode = static_cast<InstructionOpcodes>(enc_instr & kOpcodeMask);
    
    DecodedInstr decoded_instr = {
        .instr_type = {},

        .opcode = opcode,
        .instr_mnem = GetMnemonicFromOpcode(enc_instr),

        .instr = {},
    };

    switch (opcode) {
        // u_type:
        case  InstructionOpcodes::kLui:
        case InstructionOpcodes::kAuipc: {
            UTypeInstr u_type_instr = GetUTypeInstr(enc_instr);

            decoded_instr.instr_type = InstrType::UType;
            decoded_instr.instr.u_type = {
                .rd = u_type_instr.rd,
                .imm = static_cast<Register>(u_type_instr.imm), // NOTE mb << 12 should be here
                .imm_size_bit = 20,
            };
        }
        break;

        // j_type:
        case InstructionOpcodes::kJal: {
            JTypeInstr j_type_instr = GetJTypeInstr(enc_instr);

            decoded_instr.instr_type = InstrType::JType;
            decoded_instr.instr.j_type = {
                .rd = j_type_instr.rd,
                .imm = (static_cast<Register>(j_type_instr.imm_10_1 << 1)
                       + (static_cast<Register>(j_type_instr.imm_11 << 11)) 
                       + (static_cast<Register>(j_type_instr.imm_19_12 << 12)) 
                       + (static_cast<Register>(j_type_instr.imm_20 << 20))),
                .imm_size_bit = 20,
            };
        }
        break;

        // b_type:
        case InstructionOpcodes::kBranchInstr: {
            BTypeInstr b_type_instr = GetBTypeInstr(enc_instr);

            decoded_instr.instr_type = InstrType::BType;
            decoded_instr.instr.b_type = {
                .rs1 = b_type_instr.rs1,
                .rs2 = b_type_instr.rs2,
                .imm = (static_cast<Register>(b_type_instr.imm_4_1) << 1) 
                       + (static_cast<Register>(b_type_instr.imm_11) << 11) 
                       + (static_cast<Register>(b_type_instr.imm_12) << 12) 
                       + (static_cast<Register>(b_type_instr.imm_10_5) << 5),
                .imm_size_bit = 12,
            };
        }
        break;

        // i_type:
        case InstructionOpcodes::kLoadInstr:
        case InstructionOpcodes::kJalr:
        case InstructionOpcodes::kArithmImmInstr: 
        case InstructionOpcodes::kFenceInstr:
        case InstructionOpcodes::kSystemInstr: {
            ITypeInstr i_type_instr = GetITypeInstr(enc_instr);

            decoded_instr.instr_type = InstrType::IType;
            decoded_instr.instr.i_type = {
                .rd = i_type_instr.rd,
                .rs1 = i_type_instr.rs1,
                .imm = i_type_instr.imm,
                .imm_size_bit = 12,
            };
        }
        break;

        // s_type:
        case InstructionOpcodes::kStoreInstr: {
            STypeInstr s_type_instr = GetSTypeInstr(enc_instr);

            decoded_instr.instr_type = InstrType::SType;
            decoded_instr.instr.s_type = {
                .rs1 = s_type_instr.rs1,
                .rs2 = s_type_instr.rs2,
                .imm = static_cast<Register>(s_type_instr.imm_4_0) 
                       + static_cast<Register>(s_type_instr.imm_11_5 << 5u),
                .imm_size_bit = 12,
            };
        }
        break;

        // r_type:
        case InstructionOpcodes::kArithmRegInstr: {
            RTypeInstr r_type_instr = GetRTypeInstr(enc_instr);

            decoded_instr.instr_type = InstrType::RType;
            decoded_instr.instr.r_type = {
                .rd = r_type_instr.rd,
                .rs1 = r_type_instr.rs1,
                .rs2 = r_type_instr.rs2,
            };
        }
        break;

        case InstructionOpcodes::kUnknown:
        default:
            assert(0 && "Unknown instruction opcode");
    }

    return decoded_instr;
}

// static ---------------------------------------------------------------------

static InstructionMnemonic GetMnemonicFromOpcode(Register instr) {
    InstructionOpcodes opcode = static_cast<InstructionOpcodes>(instr & kOpcodeMask);

    LogVar(instr & kOpcodeMask);

    switch (opcode) {
        case InstructionOpcodes::kLui:   return InstructionMnemonic::kLui;
        case InstructionOpcodes::kAuipc: return InstructionMnemonic::kAuipc;
        case InstructionOpcodes::kJal:   return InstructionMnemonic::kJal;
        case InstructionOpcodes::kJalr:  return InstructionMnemonic::kJalr;
        
        case InstructionOpcodes::kBranchInstr: {
             BTypeInstr b_type_instr = GetBTypeInstr(instr);

            switch (static_cast< BranchInstruction>(b_type_instr.funct3)) {
                case BranchInstruction::kBeq:  return InstructionMnemonic::kBeq;
                case BranchInstruction::kBne:  return InstructionMnemonic::kBne;
                case BranchInstruction::kBlt:  return InstructionMnemonic::kBlt;
                case BranchInstruction::kBltu: return InstructionMnemonic::kBltu;
                case BranchInstruction::kBge:  return InstructionMnemonic::kBge;
                case BranchInstruction::kBgeu: return InstructionMnemonic::kBgeu;

                default:
                    assert(0 && "unkown branch instruction");
            }
        }
        break;

        case InstructionOpcodes::kLoadInstr: {
             ITypeInstr i_type_instr = GetITypeInstr(instr);
    
            switch (static_cast< LoadInstruction>(i_type_instr.funct3)) {
                case LoadInstruction::kLb:  return InstructionMnemonic::kLb;
                case LoadInstruction::kLh:  return InstructionMnemonic::kLh;
                case LoadInstruction::kLw:  return InstructionMnemonic::kLw;
                case LoadInstruction::kLbu: return InstructionMnemonic::kLbu;
                case LoadInstruction::kLhu: return InstructionMnemonic::kLhu;

                default:
                    assert(0 && "unkown load instruction");
            }
        }
        break;

        case InstructionOpcodes::kStoreInstr: {
             STypeInstr s_type_instr = GetSTypeInstr(instr);

            switch (static_cast< StoreInstruction>(s_type_instr.funct3)) { 
                case StoreInstruction::kSb: return InstructionMnemonic::kSb;
                case StoreInstruction::kSh: return InstructionMnemonic::kSh;
                case StoreInstruction::kSw: return InstructionMnemonic::kSw;

                default:
                    assert(0 && "unkown store instruction");
            }
        }
        break;

        case InstructionOpcodes::kArithmImmInstr: {
             ITypeInstr i_type_instr = GetITypeInstr(instr);

            switch (static_cast<ArithmImmInstruction>(i_type_instr.funct3)) {
                case ArithmImmInstruction::kAddi:  return InstructionMnemonic::kAddi;
                case ArithmImmInstruction::kSlti:  return InstructionMnemonic::kSlti;
                case ArithmImmInstruction::kSltiu: return InstructionMnemonic::kSltiu;
                case ArithmImmInstruction::kXori:  return InstructionMnemonic::kXori;
                case ArithmImmInstruction::kOri:   return InstructionMnemonic::kOri;
                case ArithmImmInstruction::kAndi:  return InstructionMnemonic::kAndi;
                case ArithmImmInstruction::kSlli:  return InstructionMnemonic::kSlli;
                case ArithmImmInstruction::kSraiSrli: {
                    const size_t kShmatWidth = 5;
                    ArithmImmShiftRight shift_type = static_cast<ArithmImmShiftRight>(i_type_instr.imm >> kShmatWidth);

                    switch (shift_type) {
                        case ArithmImmShiftRight::kArithm:  return InstructionMnemonic::kSrai;
                        case ArithmImmShiftRight::kLogical: return InstructionMnemonic::kSrli;
                        default:
                            assert(0 && "unknown shift");
                    }
                }

                default:
                    assert(0 && "unkown arithmetic instruction");
            }
        }
        break;

        case InstructionOpcodes::kArithmRegInstr: {
             RTypeInstr r_type_instr = GetRTypeInstr(instr);

            switch (static_cast<ArithmRegInstruction>(r_type_instr.funct3)) {
                case ArithmRegInstruction::kAddSub: {
                    switch (static_cast<ArithmRegInstructionSpecial>(r_type_instr.funct7)) {
                        case ArithmRegInstructionSpecial::kAdd: return InstructionMnemonic::kAdd;
                        case ArithmRegInstructionSpecial::kSub: return InstructionMnemonic::kSub;
                        default:
                            assert(0 && "unkown funct7 for arithmetic register operations");
                    }
                }

                case ArithmRegInstruction::kSll:  return InstructionMnemonic::kSll;
                case ArithmRegInstruction::kSlt:  return InstructionMnemonic::kSlt;
                case ArithmRegInstruction::kSltu: return InstructionMnemonic::kSltu;
                case ArithmRegInstruction::kXor:  return InstructionMnemonic::kXor;
                
                case ArithmRegInstruction::kSrlSra: {
                    switch (static_cast<ArithmRegInstructionSpecial>(r_type_instr.funct7)) {
                        case ArithmRegInstructionSpecial::kSrl: return InstructionMnemonic::kSrl;
                        case ArithmRegInstructionSpecial::kSra: return InstructionMnemonic::kSra;
                        default:
                            assert(0 && "unkown funct7 for arithmetic register operations");
                    }
                }
                
                case ArithmRegInstruction::kOr:  return InstructionMnemonic::kOr;
                case ArithmRegInstruction::kAnd: return InstructionMnemonic::kAnd;
            }
        }
        break;

        case InstructionOpcodes::kFenceInstr: {
            assert(0 && "fence not implemented yet");
        }
        break;

        case InstructionOpcodes::kSystemInstr: {
            // assert(0 && "system instructions not implemented yet");
            ITypeInstr i_type_instr = GetITypeInstr(instr);
            switch (static_cast<SystemInstruction>(i_type_instr.funct3)) {
                case SystemInstruction::kScallSbreak: {
                    switch (static_cast<SystemInstructionSpecial>(i_type_instr.imm)) {
                        case SystemInstructionSpecial::kScall: return InstructionMnemonic::kScall;
                        case SystemInstructionSpecial::kSbreak: return InstructionMnemonic::kSbreak;
                        
                        default:
                            assert(0 && "unknown system instruction");
                    }
                }
                case SystemInstruction::kSread: {
                    assert(0 && "fence not implemented");
                }
            }
        }
        break;

        case InstructionOpcodes::kUnknown:
        default:
            spdlog::critical("Unknown opcode");
            assert(0 && "unkown opcode");
    }

    return InstructionMnemonic::kUnkownMnem;
}

static RTypeInstr GetRTypeInstr(const Register instr) {
    // LogFunctionEntry();

    RTypeInstr r_type_instr = {};
    std::memcpy(&r_type_instr, &instr, sizeof(instr));

    // LogVar(static_cast<Register>(r_type_instr.opcode));
    // LogVar(static_cast<Register>(r_type_instr.rd));
    // LogVar(static_cast<Register>(r_type_instr.funct3));
    // LogVar(static_cast<Register>(r_type_instr.rs1));
    // LogVar(static_cast<Register>(r_type_instr.rs2));
    // LogVar(static_cast<Register>(r_type_instr.funct7));

    return r_type_instr;
}

static ITypeInstr GetITypeInstr(const Register instr) {
    // LogFunctionEntry();

    ITypeInstr i_type_instr = {};
    std::memcpy(&i_type_instr, &instr, sizeof(instr));

    // LogVar(static_cast<Register>(i_type_instr.opcode));
    // LogVar(static_cast<Register>(i_type_instr.rd));
    // LogVar(static_cast<Register>(i_type_instr.funct3));
    // LogVar(static_cast<Register>(i_type_instr.rs1));
    // LogVar(static_cast<Register>(i_type_instr.imm));

    return i_type_instr;
}

static STypeInstr GetSTypeInstr(const Register instr) {
    // LogFunctionEntry();

    STypeInstr s_type_instr = {};
    std::memcpy(&s_type_instr, &instr, sizeof(instr));

    // LogVar(static_cast<Register>(s_type_instr.opcode));
    // LogVar(static_cast<Register>(s_type_instr.imm_4_0));
    // LogVar(static_cast<Register>(s_type_instr.funct3));
    // LogVar(static_cast<Register>(s_type_instr.rs1));
    // LogVar(static_cast<Register>(s_type_instr.rs2));
    // LogVar(static_cast<Register>(s_type_instr.imm_11_5));

    return s_type_instr;
}

static BTypeInstr GetBTypeInstr(const Register instr) {
    // LogFunctionEntry();

    BTypeInstr b_type_instr = {};
    std::memcpy(&b_type_instr, &instr, sizeof(instr));

    // LogVar(static_cast<Register>(b_type_instr.opcode));
    // LogVar(static_cast<Register>(b_type_instr.imm_11));
    // LogVar(static_cast<Register>(b_type_instr.imm_4_1));
    // LogVar(static_cast<Register>(b_type_instr.funct3));
    // LogVar(static_cast<Register>(b_type_instr.rs1));
    // LogVar(static_cast<Register>(b_type_instr.rs2));
    // LogVar(static_cast<Register>(b_type_instr.imm_10_5));
    // LogVar(static_cast<Register>(b_type_instr.imm_12));

    return b_type_instr;
}

static UTypeInstr GetUTypeInstr(const Register instr) {
    // LogFunctionEntry();

    UTypeInstr u_type_instr = {};
    std::memcpy(&u_type_instr, &instr, sizeof(instr));

    // LogVar(static_cast<Register>(u_type_instr.opcode));
    // LogVar(static_cast<Register>(u_type_instr.rd));
    // LogVar(static_cast<Register>(u_type_instr.imm));

    return u_type_instr;
}

static JTypeInstr GetJTypeInstr(const Register instr) {
    // LogFunctionEntry();

    JTypeInstr j_type_instr = {};
    std::memcpy(&j_type_instr, &instr, sizeof(instr));

    // LogVar(static_cast<Register>(j_type_instr.opcode));
    // LogVar(static_cast<Register>(j_type_instr.rd));
    // LogVar(static_cast<Register>(j_type_instr.imm_19_12));
    // LogVar(static_cast<Register>(j_type_instr.imm_11));
    // LogVar(static_cast<Register>(j_type_instr.imm_10_1));
    // LogVar(static_cast<Register>(j_type_instr.imm_20));

    return j_type_instr;
}

} // namespace sim
