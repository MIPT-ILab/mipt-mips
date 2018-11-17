/**
 * mips_isa.cpp - instruction decoder for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include <func_sim/alu.h>
#include <infra/types.h>
#include <infra/macro.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "mips_instr.h"
#include "mips_instr_decode.h"

enum class RegType : uint8
{
    RS, RT, RD,
    ZERO, RA,
    HI, LO, HI_LO
};

bool is_explicit_register( RegType type)
{
    return type == RegType::RS
        || type == RegType::RT
        || type == RegType::RD;
}

template<typename R>
struct MIPSTableEntry
{
    std::string_view name;
    OperationType operation;
    uint8 mem_size;
    RegType src1;
    RegType src2;
    RegType dst;
    ALU::Execute<BaseMIPSInstr<R>> function;
    MIPSVersionMask versions;
    MIPSTableEntry() = delete;
};

MIPSRegister get_register( const MIPSInstrDecoder& instr, RegType type)
{
    switch ( type) {
    case RegType::HI:    return MIPSRegister::mips_hi;
    case RegType::LO:    return MIPSRegister::mips_lo;
    case RegType::HI_LO: return MIPSRegister::mips_lo;
    case RegType::ZERO:  return MIPSRegister::zero;
    case RegType::RA:    return MIPSRegister::return_address;
    case RegType::RS:    return MIPSRegister::from_cpu_index( instr.rs);
    case RegType::RT:    return MIPSRegister::from_cpu_index( instr.rt);
    case RegType::RD:    return MIPSRegister::from_cpu_index( instr.rd);
    default: assert(0);  return MIPSRegister::zero;
    }
}

template<typename R>
using Table = std::unordered_map<uint32, MIPSTableEntry<R>>;

//unordered map for R-instructions
template<typename R>
static const Table<R> isaMapR =
{
    // **************** R INSTRUCTIONS ****************
    // Constant shifts
    //key     name    operation  ms  source1      source2      destination      pointer                              mips version
    {0x0, { "sll" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, ALU::sll<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr} },
    //       0x1 movci
    {0x2, { "srl", OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, ALU::srl<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr} },
    {0x3, { "sra", OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, ALU::sra<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr} },

    // Variable shifts
    //key     name    operation   ms  source1      source2      destination      pointer                       mips version
    {0x4, { "sllv", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, ALU::sllv<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr} },
    //        0x5 reserved
    {0x6, { "srlv", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, ALU::srlv<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr} },
    {0x7, { "srav", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, ALU::srav<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr} },

    // Indirect branches
    //key     name    operation  ms  source1      source2      destination      pointer                           mips version
    {0x8, { "jr"  , OUT_R_JUMP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::jr<BaseMIPSInstr<R>>,   MIPS_I_Instr} },
    {0x9, { "jalr", OUT_R_JUMP, 0, RegType::RS, RegType::ZERO, RegType::RD,
                               ALU::jump_and_link<BaseMIPSInstr<R>, ALU::jr<BaseMIPSInstr<R>>>, MIPS_I_Instr} },

    // Conditional moves (MIPS IV)
    //key     name    operation   ms  source1      source2      destination      pointer                        mips version
    {0xA,  { "movz", OUT_R_CONDM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::movz<BaseMIPSInstr<R>>, MIPS_IV_Instr | MIPS_32_Instr} },
    {0xB,  { "movn", OUT_R_CONDM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::movn<BaseMIPSInstr<R>>, MIPS_IV_Instr | MIPS_32_Instr} },

    // System calls
    //key     name    operation       ms     source1       source2      destination      pointer                        mips version
    {0xC, { "syscall", OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, ALU::syscall<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    {0xD, { "break",   OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, ALU::breakpoint<BaseMIPSInstr<R>>,   MIPS_I_Instr} },
    //          0xE reserved
    //          0xF SYNC

    // HI/LO manipulations
    //key     name    operation  ms     source1       source2   destination      pointer                      mips version
    {0x10, { "mfhi", OUT_R_ARITHM, 0, RegType::HI, RegType::ZERO, RegType::RD, ALU::move<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    {0x11, { "mthi", OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::HI, ALU::move<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    {0x12, { "mflo", OUT_R_ARITHM, 0, RegType::LO, RegType::ZERO, RegType::RD, ALU::move<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    {0x13, { "mtlo", OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::LO, ALU::move<BaseMIPSInstr<R>>, MIPS_I_Instr} },

    // Doubleword variable shifts
    //key     name    operation  ms     source1       source2   destination       pointer                             mips version
    {0x14, { "dsllv", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, ALU::sllv<BaseMIPSInstr<R>, uint64>, MIPS_III_Instr} },
    {0x16, { "dsrlv", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, ALU::srlv<BaseMIPSInstr<R>, uint64>, MIPS_III_Instr} },
    {0x17, { "dsrav", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, ALU::srav<BaseMIPSInstr<R>, uint64>, MIPS_III_Instr} },

    // Multiplication/Division
    //key     name    operation  ms     source1       source2        destination       pointer                                       mips version
    {0x18, { "mult",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::multiplication<BaseMIPSInstr<R>, int32>,  MIPS_I_Instr} },
    {0x19, { "multu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::multiplication<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr} },
    {0x1A, { "div",   OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::division<BaseMIPSInstr<R>, int32>,        MIPS_I_Instr} },
    {0x1B, { "divu",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::division<BaseMIPSInstr<R>, uint32>,       MIPS_I_Instr} },
    {0x1C, { "dmult", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::multiplication<BaseMIPSInstr<R>, int64>,  MIPS_III_Instr} },
    {0x1D, { "dmultu",OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::multiplication<BaseMIPSInstr<R>, uint64>, MIPS_III_Instr} },
    {0x1E, { "ddiv",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::division<BaseMIPSInstr<R>, int64>,        MIPS_III_Instr} },
    {0x1F, { "ddivu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::division<BaseMIPSInstr<R>, uint64>,       MIPS_III_Instr} },

    // Addition/Subtraction
    //key     name    operation    ms   source1       source2   destination       pointer                             mips version
    {0x20, { "add",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::addition<BaseMIPSInstr<R>, int32>,     MIPS_I_Instr} },
    {0x21, { "addu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::addition<BaseMIPSInstr<R>, uint32>,    MIPS_I_Instr} },
    {0x22, { "sub",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::subtraction<BaseMIPSInstr<R>, int32>,  MIPS_I_Instr} },
    {0x23, { "subu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::subtraction<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr} },

    // Logical operations
    //key     name    operation   ms   source1       source2   destination       pointer                       mips version
    {0x24, { "and", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::andv<BaseMIPSInstr<R>>,  MIPS_I_Instr} },
    {0x25, { "or",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::orv<BaseMIPSInstr<R>>,   MIPS_I_Instr} },
    {0x26, { "xor", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::xorv<BaseMIPSInstr<R>>,  MIPS_I_Instr} },
    {0x27, { "nor", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::nor<BaseMIPSInstr<R>>,   MIPS_I_Instr} },
    //        0x28 reserved
    //        0x29 reserved
    {0x2A, { "slt",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::set<BaseMIPSInstr<R>, ALU::lt<BaseMIPSInstr<R>>>,  MIPS_I_Instr} },
    {0x2B, { "sltu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::set<BaseMIPSInstr<R>, ALU::ltu<BaseMIPSInstr<R>>>, MIPS_I_Instr} },

    // Doubleword addition/Subtraction
    //key     name    operation   ms   source1       source2     destination       pointer                                  mips version
    {0x2C, { "dadd",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::addition<BaseMIPSInstr<R>, int64>,     MIPS_I_Instr} },
    {0x2D, { "daddu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::addition<BaseMIPSInstr<R>, uint64>,    MIPS_I_Instr} },
    {0x2E, { "dsub",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::subtraction<BaseMIPSInstr<R>, int64>,  MIPS_III_Instr} },
    {0x2F, { "dsubu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, ALU::subtraction<BaseMIPSInstr<R>, uint64>, MIPS_III_Instr} },

    // Conditional traps (MIPS II)
    //key     name    operation  ms   source1       source2     destination       pointer                                                    mips version
    {0x30, { "tge",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::ge<BaseMIPSInstr<R>>>,  MIPS_II_Instr} },
    {0x31, { "tgeu", OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::geu<BaseMIPSInstr<R>>>, MIPS_II_Instr} },
    {0x32, { "tlt",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::lt<BaseMIPSInstr<R>>>,  MIPS_II_Instr} },
    {0x33, { "tltu", OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::ltu<BaseMIPSInstr<R>>>, MIPS_II_Instr} },
    {0x34, { "teq",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::eq<BaseMIPSInstr<R>>>,  MIPS_II_Instr} },
    //        0x35 reserved
    {0x36, { "tne",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::ne<BaseMIPSInstr<R>>>,   MIPS_II_Instr} },
    //        0x37 reserved

    // Doubleword shifts
    //key     name    operation     ms   source1       source2     destination       pointer                           mips version
    {0x38, { "dsll"   , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, ALU::sll<BaseMIPSInstr<R>, uint64>, MIPS_III_Instr} },
    {0x3A, { "dsrl"   , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, ALU::srl<BaseMIPSInstr<R>, uint64>, MIPS_III_Instr} },
    {0x3B, { "dsra"   , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, ALU::sra<BaseMIPSInstr<R>, uint64>, MIPS_III_Instr} },
    {0x3C, { "dsll32" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, ALU::dsll32<BaseMIPSInstr<R>>,      MIPS_III_Instr} },
    {0x3E, { "dsrl32" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, ALU::dsrl32<BaseMIPSInstr<R>>,      MIPS_III_Instr} },
    {0x3F, { "dsra32" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, ALU::dsra32<BaseMIPSInstr<R>>,      MIPS_III_Instr} }
};

//unordered map for RI-instructions
template<typename R>
static const Table<R> isaMapRI =
{
// ********************** REGIMM INSTRUCTIONS *************************
    // Branches
    //key     name    operation   memsize    source1        source2      destination      pointer                                                       mips version
    {0x0,  { "bltz",  OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::ltz<BaseMIPSInstr<R>>>, MIPS_I_Instr} },
    {0x1,  { "bgez",  OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::gez<BaseMIPSInstr<R>>>, MIPS_I_Instr} },
    {0x2,  { "bltzl", OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::ltz<BaseMIPSInstr<R>>>, MIPS_II_Instr} },
    {0x3,  { "bgezl", OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::gez<BaseMIPSInstr<R>>>, MIPS_II_Instr} },

    // Traps
    //key     name    operation    ms  source1        source2      destination      pointer                                                       mips version
    {0x8,  { "tgei",  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::gei<BaseMIPSInstr<R>>>,  MIPS_II_Instr} },
    {0x9,  { "tgeiu", OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::geiu<BaseMIPSInstr<R>>>, MIPS_II_Instr} },
    {0xA,  { "tlti",  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::lti<BaseMIPSInstr<R>>>,  MIPS_II_Instr} },
    {0xB,  { "tltiu", OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::ltiu<BaseMIPSInstr<R>>>, MIPS_II_Instr} },
    {0xC,  { "teqi",  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::eqi<BaseMIPSInstr<R>>>,  MIPS_II_Instr} },
    {0xE,  { "tnei",  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::trap<BaseMIPSInstr<R>, ALU::nei<BaseMIPSInstr<R>>>,  MIPS_II_Instr} },

    // Linking branches
    //key     name    operation   memsize     source1        source2      destination      pointer                                                             mips version
    {0x10, { "bltzal",  OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::RA, ALU::branch_and_link<BaseMIPSInstr<R>, ALU::ltz<BaseMIPSInstr<R>>>, MIPS_I_Instr} },
    {0x11, { "bgezal",  OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::RA, ALU::branch_and_link<BaseMIPSInstr<R>, ALU::gez<BaseMIPSInstr<R>>>, MIPS_I_Instr} },
    {0x12, { "bltzall", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::RA, ALU::branch_and_link<BaseMIPSInstr<R>, ALU::ltz<BaseMIPSInstr<R>>>, MIPS_II_Instr} },
    {0x13, { "bgezall", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::RA, ALU::branch_and_link<BaseMIPSInstr<R>, ALU::gez<BaseMIPSInstr<R>>>, MIPS_II_Instr} }
};

//unordered map for I-instructions and J-instructions
template<typename R>
static const Table<R> isaMapIJ =
{
    // ********************* I and J INSTRUCTIONS *************************
    // Direct jumps
    //key     name    operation  memsize    source1      source2      destination      pointer                          mips version
    {0x2, { "j",   OUT_J_JUMP, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, ALU::j<BaseMIPSInstr<R>>,     MIPS_I_Instr } },
    {0x3, { "jal", OUT_J_JUMP, 0, RegType::ZERO, RegType::ZERO, RegType::RA,
                                ALU::jump_and_link<BaseMIPSInstr<R>, ALU::j<BaseMIPSInstr<R>> >,   MIPS_I_Instr } },

    // Branches
    //key     name    operation  memsize    source1      source2      destination      pointer                                                     mips version
    {0x4, { "beq",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::eq<BaseMIPSInstr<R>>>,  MIPS_I_Instr} },
    {0x5, { "bne",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::ne<BaseMIPSInstr<R>>>,  MIPS_I_Instr} },
    {0x6, { "blez", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::lez<BaseMIPSInstr<R>>>, MIPS_I_Instr} },
    {0x7, { "bgtz", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::gtz<BaseMIPSInstr<R>>>, MIPS_I_Instr} },

    // Addition/Subtraction
    //key     name    operation  memsize    source1      source2   destination      pointer                                     mips version
    {0x8, { "addi",  OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, ALU::addition_imm<BaseMIPSInstr<R>, int32>,  MIPS_I_Instr} },
    {0x9, { "addiu", OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, ALU::addition_imm<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr} },

    // Logical operations
    //key     name    operation  memsize    source1      source2      destination      pointer                                                     mips version
    {0xA, { "slti",  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, ALU::set<BaseMIPSInstr<R>, ALU::lti<BaseMIPSInstr<R>>>,  MIPS_I_Instr} },
    {0xB, { "sltiu", OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, ALU::set<BaseMIPSInstr<R>, ALU::ltiu<BaseMIPSInstr<R>>>, MIPS_I_Instr} },
    {0xC, { "andi",  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, ALU::andi<BaseMIPSInstr<R>>,   MIPS_I_Instr} },
    {0xD, { "ori",   OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, ALU::ori<BaseMIPSInstr<R>>,    MIPS_I_Instr} },
    {0xE, { "xori",  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, ALU::xori<BaseMIPSInstr<R>>,   MIPS_I_Instr} },
    {0xF, { "lui",   OUT_I_CONST,  0, RegType::ZERO, RegType::ZERO, RegType::RT, ALU::lui<BaseMIPSInstr<R>>,    MIPS_I_Instr} },

    // 0x10 - 0x13 coprocessor operations

    // Likely branches (MIPS II)
    //key     name    operation   memsize    source1      source2      destination      pointer                                                           mips version
    {0x14, { "beql",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::eq<BaseMIPSInstr<R>>>,  MIPS_II_Instr} },
    {0x15, { "bnel",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::ne<BaseMIPSInstr<R>>>,  MIPS_II_Instr} },
    {0x16, { "blezl", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::RT,   RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::lez<BaseMIPSInstr<R>>>, MIPS_II_Instr} },
    {0x17, { "bgtzl", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::RT,   RegType::ZERO, ALU::branch<BaseMIPSInstr<R>, ALU::gtz<BaseMIPSInstr<R>>>, MIPS_II_Instr} },

    // Doubleword unaligned loads
    //key     name    operation  memsize    source1      source2      destination      pointer                         mips version
    {0x1A, { "ldl",  OUT_I_LOAD, 8, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr<BaseMIPSInstr<R>>, MIPS_III_Instr} },
    {0x1B, { "ldr",  OUT_I_LOAD, 8, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr<BaseMIPSInstr<R>>, MIPS_III_Instr} },

    // Doubleword addition
    //key     name    operation  memsize    source1      source2     destination      pointer                                        mips version
    {0x18, { "daddi",  OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, ALU::addition_imm<BaseMIPSInstr<R>, int64>,  MIPS_III_Instr} },
    {0x19, { "daddiu", OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, ALU::addition_imm<BaseMIPSInstr<R>, uint64>, MIPS_III_Instr} },

    // Loads
    //key     name    operation     memsize    source1      source2      destination      pointer                                 mips version
    {0x20, { "lb",  OUT_I_LOAD,         1, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr<BaseMIPSInstr<R>>,         MIPS_I_Instr} },
    {0x21, { "lh",  OUT_I_LOAD,         2, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr_aligned<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    {0x22, { "lwl", OUT_I_PARTIAL_LOAD, 4, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr_left32<BaseMIPSInstr<R>>,  MIPS_I_Instr} },
    {0x23, { "lw",  OUT_I_LOAD,         4, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr_aligned<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    {0x24, { "lbu", OUT_I_LOADU,        1, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr<BaseMIPSInstr<R>>,         MIPS_I_Instr} },
    {0x25, { "lhu", OUT_I_LOADU,        2, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr_aligned<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    {0x26, { "lwr", OUT_I_PARTIAL_LOAD, 4, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr_right32<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    {0x27, { "lwu", OUT_I_LOADU,        4, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr_aligned<BaseMIPSInstr<R>>, MIPS_I_Instr} },

    // Stores
    //key     name    operation  memsize    source1      source2      destination      pointer                                         mips version
    {0x28, { "sb",  OUT_I_STORE, 1, RegType::RS, RegType::RT, RegType::ZERO, ALU::store_addr<BaseMIPSInstr<R>>,         MIPS_I_Instr} },
    {0x29, { "sh",  OUT_I_STORE, 2, RegType::RS, RegType::RT, RegType::ZERO, ALU::store_addr<BaseMIPSInstr<R>>,         MIPS_I_Instr} },
    {0x2A, { "swl", OUT_I_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, ALU::store_addr_left32<BaseMIPSInstr<R>>,  MIPS_I_Instr} },
    {0x2B, { "sw",  OUT_I_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, ALU::store_addr_aligned<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    {0x2C, { "sdl", OUT_I_STORE, 8, RegType::RS, RegType::RT, RegType::ZERO, ALU::store_addr<BaseMIPSInstr<R>>,         MIPS_III_Instr} },
    {0x2D, { "sdr", OUT_I_STORE, 8, RegType::RS, RegType::RT, RegType::ZERO, ALU::store_addr<BaseMIPSInstr<R>>,         MIPS_III_Instr} },
    {0x2E, { "swr", OUT_I_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, ALU::store_addr_right32<BaseMIPSInstr<R>>, MIPS_I_Instr} },
    //       0x2F   cache

    // Advanced loads and stores
    {0x30, { "ll",  OUT_I_LOAD,   2, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr<BaseMIPSInstr<R>>,  MIPS_I_Instr} },

    {0x37, { "ld",  OUT_I_LOAD,   8, RegType::RS, RegType::ZERO, RegType::RT, ALU::load_addr<BaseMIPSInstr<R>>,  MIPS_III_Instr} },
    {0x38, { "sc",  OUT_I_STORE,  2, RegType::RS, RegType::RT, RegType::ZERO, ALU::store_addr<BaseMIPSInstr<R>>, MIPS_I_Instr} },

    {0x3F, { "sd",  OUT_I_STORE,  8, RegType::RS, RegType::RT, RegType::ZERO, ALU::store_addr<BaseMIPSInstr<R>>, MIPS_III_Instr} }
};

template<typename R>
static const Table<R> isaMapMIPS32 =
{
    // ********************* MIPS32 INSTRUCTIONS *************************
    // Advanced multiplication
    //key     name    operation  memsize    source1      source2      destination      pointer                                         mips version
    {0x00, { "madd",  OUT_R_ACCUM,   0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::multiplication<BaseMIPSInstr<R>, int32>,  MIPS_32_Instr} },
    {0x01, { "maddu", OUT_R_ACCUM,   0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::multiplication<BaseMIPSInstr<R>, uint32>, MIPS_32_Instr} },
    {0x02, { "mul",   OUT_R_ARITHM,  0, RegType::RS, RegType::RT, RegType::RD,    ALU::multiplication<BaseMIPSInstr<R>, int32>,  MIPS_32_Instr} },
    {0x04, { "msub",  OUT_R_SUBTR,   0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::multiplication<BaseMIPSInstr<R>, int32>,  MIPS_32_Instr} },
    {0x05, { "msubu", OUT_R_SUBTR,   0, RegType::RS, RegType::RT, RegType::HI_LO, ALU::multiplication<BaseMIPSInstr<R>, uint32>, MIPS_32_Instr} },

    // Count leading zeroes/ones
    //key     name    operation  memsize    source1      source2      destination      pointer                       mips version
    {0x20, { "clz",   OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, ALU::clz<BaseMIPSInstr<R>>,     MIPS_32_Instr} },
    {0x21, { "clo",   OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, ALU::clo<BaseMIPSInstr<R>>,     MIPS_32_Instr} },
    {0x24, { "dclz",  OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, ALU::dclz<BaseMIPSInstr<R>>,    MIPS_64_Instr} },
    {0x25, { "dclo",  OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, ALU::dclo<BaseMIPSInstr<R>>,    MIPS_64_Instr} }
};

template<typename R>
MIPSTableEntry<R> unknown_instruction =
{ "Unknown instruction", OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, unknown_mips_instruction, MIPS_I_Instr};

template<typename R>
MIPSTableEntry<R> nop =
{ "nop" , OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, ALU::sll<BaseMIPSInstr<R>, uint32>, MIPS_I_Instr};

template<typename R>
const MIPSTableEntry<R>& get_table_entry( const Table<R>& table, uint32 key)
{
    auto it = table.find( key);
    return it == table.end() ? unknown_instruction<R> : it->second;
}

template<typename R>
const MIPSTableEntry<R>& get_table_entry( uint32 bytes)
{
    MIPSInstrDecoder instr( bytes);

    if ( instr.bytes == 0)
        return nop<R>;

    switch ( instr.opcode)
    {
        case 0x0:  return get_table_entry( isaMapR<R>,      instr.funct);  // R instruction
        case 0x1:  return get_table_entry( isaMapRI<R>,     instr.rt);     // RegIMM instruction
        case 0x1C: return get_table_entry( isaMapMIPS32<R>, instr.funct);  // MIPS32 instruction
        default:   return get_table_entry( isaMapIJ<R>,     instr.opcode); // I and J instructions
    }
}

template<typename M>
auto find_entry( const M& map, std::string_view name)
{
    return std::find_if( map.begin(), map.end(), [name]( const auto& e) {
        return e.second.name == name;
    });
}

template<typename R>
const MIPSTableEntry<R>& get_table_entry( std::string_view str_opcode)
{
    if ( str_opcode == "nop")
        return nop<R>;

    for ( const auto& map : { isaMapR<R>, isaMapRI<R>, isaMapMIPS32<R>, isaMapIJ<R> })
    {
        auto res = find_entry( map, str_opcode);
        if ( res != map.end())
            return res->second;
    }

    return unknown_instruction<R>;
}

template<typename R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, uint32 bytes, Addr PC) :
    raw( bytes),
    new_PC( PC + 4),
    PC( PC)
{
    init( get_table_entry<R>( raw), version);
}

template<typename R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, Addr PC)
    : raw( 0)
    , new_PC( PC + 4)
    , PC( PC)
{
    init( get_table_entry<R>( str_opcode), version);
}

template<typename R>
void BaseMIPSInstr<R>::init( const MIPSTableEntry<R>& entry, MIPSVersion version)
{
    MIPSInstrDecoder instr( raw);

    operation = entry.operation;
    mem_size  = entry.mem_size;
    execute_function = entry.versions.is_supported(version)
        ? entry.function
        : unknown_mips_instruction<BaseMIPSInstr<R>>;

    shamt = instr.shamt;

    src1 = get_register( instr, entry.src1);
    src2 = get_register( instr, entry.src2);
    dst  = get_register( instr, entry.dst);
    if ( entry.dst == RegType::HI_LO)
        dst2 = MIPSRegister::mips_hi;

    const bool print_dst  = is_explicit_register( entry.dst);
    const bool print_src1 = is_explicit_register( entry.src1);
    const bool print_src2 = is_explicit_register( entry.src2);

    std::ostringstream oss;
    if ( PC != 0)
        oss << std::hex << "0x" << PC << ": ";
    oss << entry.name;

    switch ( operation)
    {
        case OUT_R_SHAMT:
            oss <<  " $" << dst
                << ", $" << src1
                <<  ", " << std::dec << shamt;
            break;
        case OUT_I_ARITHM:
            v_imm = instr.imm;

            oss << " $" << dst << ", $"
                << src1 << ", "
                << std::hex << "0x" << v_imm << std::dec;
            break;
        case OUT_I_BRANCH:
            v_imm = instr.imm;

            oss << " $" << src1 << ", $"
                << src2 << ", "
                << std::dec << narrow_cast<int16>(v_imm);
            break;
        case OUT_RI_BRANCH_0:
            v_imm = instr.imm;
            oss << " $" << src1 << ", "
                << std::dec << narrow_cast<int16>(v_imm);
            break;
        case OUT_RI_TRAP:
            v_imm = instr.imm;
            oss << " $" << src1 << ", 0x"
                << std::hex << narrow_cast<int16>(v_imm) << std::dec;
            break;
        case OUT_I_CONST:
            v_imm = instr.imm;

            oss << " $" << dst << std::hex
                << ", 0x" << v_imm << std::dec;
            break;

        case OUT_I_LOAD:
        case OUT_I_LOADU:
        case OUT_I_PARTIAL_LOAD:
            v_imm = instr.imm;

            oss << " $" << dst << ", 0x"
                << std::hex << v_imm
                << "($" << src1 << ")" << std::dec;
            break;

        case OUT_I_STORE:
            v_imm = instr.imm;

            oss << " $" << src2 << ", 0x"
                << std::hex << v_imm
                << "($" << src1 << ")" << std::dec;
            break;
        case OUT_J_JUMP:
            v_imm = instr.jump;
            oss << " 0x" << std::hex << v_imm << std::dec;
            break;
        default:
            if (print_dst) {
                oss <<  " $" << dst;
                if (print_src1)
                    oss << ",";
            }
            if (print_src1)
                oss <<  " $" << src1;
            if (print_src2)
                oss << ", $" << src2;
            break;
    }

    disasm = oss.str();
}

template class BaseMIPSInstr<uint32>;
template class BaseMIPSInstr<uint64>;
