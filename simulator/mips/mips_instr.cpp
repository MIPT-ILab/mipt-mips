/*
 * mips_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "mips_instr.h"
#include "mips_instr_decode.h"

#include <iomanip>
#include <iostream>
#include <sstream>

enum class RegType : uint8
{
    RS, RT, RD,
    ZERO, RA,
    HI, LO, HI_LO
};

static bool is_explicit_register( RegType type)
{
    return type == RegType::RS
        || type == RegType::RT
        || type == RegType::RD;
}

template<typename RegisterUInt>
struct MIPSTableEntry
{
    std::string_view name;
    OperationType operation;
    uint8 mem_size;
    RegType src1;
    RegType src2;
    RegType dst;
    typename BaseMIPSInstr<RegisterUInt>::Execute function;
    MIPSVersionMask versions;
    MIPSTableEntry() = delete;
};

static MIPSRegister get_register( const MIPSInstrDecoder& instr, RegType type)
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

template<typename RegisterUInt>
using Table = std::unordered_map<uint8, MIPSTableEntry<RegisterUInt>>;

//unordered map for R-instructions
template<typename RegisterUInt>
static const Table<RegisterUInt> isaMapR =
{
    // **************** R INSTRUCTIONS ****************
    // Constant shifts
    //key     name    operation  ms  source1      source2      destination      pointer                              mips version
    {0x0, { "sll" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_sll<uint32>, MIPS_I_Instr} },
    //       0x1 movci
    {0x2, { "srl", OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_srl<uint32>, MIPS_I_Instr} },
    {0x3, { "sra", OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_sra<uint32>, MIPS_I_Instr} },

    // Variable shifts
    //key     name    operation   ms  source1      source2      destination      pointer                       mips version
    {0x4, { "sllv", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_sllv<uint32>, MIPS_I_Instr} },
    //        0x5 reserved
    {0x6, { "srlv", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_srlv<uint32>, MIPS_I_Instr} },
    {0x7, { "srav", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_srav<uint32>, MIPS_I_Instr} },

    // Indirect branches
    //key     name    operation  ms  source1      source2      destination      pointer                           mips version
    {0x8, { "jr"  , OUT_R_JUMP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::execute_jr,   MIPS_I_Instr} },
    {0x9, { "jalr", OUT_R_JUMP, 0, RegType::RS, RegType::ZERO, RegType::RD,
                               &BaseMIPSInstr<RegisterUInt>::template execute_jump_and_link<&BaseMIPSInstr<RegisterUInt>::execute_jr>, MIPS_I_Instr} },

    // Conditional moves (MIPS IV)
    //key     name    operation   ms  source1      source2      destination      pointer                        mips version
    {0xA,  { "movz", OUT_R_CONDM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_movz, MIPS_IV_Instr | MIPS_32_Instr} },
    {0xB,  { "movn", OUT_R_CONDM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_movn, MIPS_IV_Instr | MIPS_32_Instr} },

    // System calls
    //key     name    operation       ms     source1       source2      destination      pointer                        mips version
    {0xC, { "syscall", OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::execute_syscall, MIPS_I_Instr} },
    {0xD, { "break",   OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::execute_break,   MIPS_I_Instr} },
    //          0xE reserved
    //          0xF SYNC

    // HI/LO manipulations
    //key     name    operation  ms     source1       source2   destination      pointer                      mips version
    {0x10, { "mfhi", OUT_R_ARITHM, 0, RegType::HI, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_move, MIPS_I_Instr} },
    {0x11, { "mthi", OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::HI, &BaseMIPSInstr<RegisterUInt>::execute_move, MIPS_I_Instr} },
    {0x12, { "mflo", OUT_R_ARITHM, 0, RegType::LO, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_move, MIPS_I_Instr} },
    {0x13, { "mtlo", OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::LO, &BaseMIPSInstr<RegisterUInt>::execute_move, MIPS_I_Instr} },

    // Doubleword variable shifts
    //key     name    operation  ms     source1       source2   destination       pointer                             mips version
    {0x14, { "dsllv", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_sllv<uint64>, MIPS_III_Instr} },
    {0x16, { "dsrlv", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_srlv<uint64>, MIPS_III_Instr} },
    {0x17, { "dsrav", OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_srav<uint64>, MIPS_III_Instr} },

    // Multiplication/Division
    //key     name    operation  ms     source1       source2        destination       pointer                                       mips version
    {0x18, { "mult",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_multiplication<int32>,  MIPS_I_Instr} },
    {0x19, { "multu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_multiplication<uint32>, MIPS_I_Instr} },
    {0x1A, { "div",   OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_division<int32>,        MIPS_I_Instr} },
    {0x1B, { "divu",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_division<uint32>,       MIPS_I_Instr} },
    {0x1C, { "dmult", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_multiplication<int64>,  MIPS_III_Instr} },
    {0x1D, { "dmultu",OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_multiplication<uint64>, MIPS_III_Instr} },
    {0x1E, { "ddiv",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_division<int64>,        MIPS_III_Instr} },
    {0x1F, { "ddivu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_division<uint64>,       MIPS_III_Instr} },

    // Addition/Subtraction
    //key     name    operation    ms   source1       source2   destination       pointer                             mips version
    {0x20, { "add",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_addition<int32>,     MIPS_I_Instr} },
    {0x21, { "addu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_addition<uint32>,    MIPS_I_Instr} },
    {0x22, { "sub",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_subtraction<int32>,  MIPS_I_Instr} },
    {0x23, { "subu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_subtraction<uint32>, MIPS_I_Instr} },

    // Logical operations
    //key     name    operation   ms   source1       source2   destination       pointer                       mips version
    {0x24, { "and", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_and,  MIPS_I_Instr} },
    {0x25, { "or",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_or,   MIPS_I_Instr} },
    {0x26, { "xor", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_xor,  MIPS_I_Instr} },
    {0x27, { "nor", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_nor,  MIPS_I_Instr} },
    //        0x28 reserved
    //        0x29 reserved
    {0x2A, { "slt",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_set<&BaseMIPSInstr<RegisterUInt>::lt>,  MIPS_I_Instr} },
    {0x2B, { "sltu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_set<&BaseMIPSInstr<RegisterUInt>::ltu>, MIPS_I_Instr} },

    // Doubleword addition/Subtraction
    //key     name    operation   ms   source1       source2     destination       pointer                                  mips version
    {0x2C, { "dadd",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_addition<int64>,     MIPS_I_Instr} },
    {0x2D, { "daddu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_addition<uint64>,    MIPS_I_Instr} },
    {0x2E, { "dsub",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_subtraction<int64>,  MIPS_III_Instr} },
    {0x2F, { "dsubu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_subtraction<uint64>, MIPS_III_Instr} },

    // Conditional traps (MIPS II)
    //key     name    operation  ms   source1       source2     destination       pointer                                                    mips version
    {0x30, { "tge",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::ge>,  MIPS_II_Instr} },
    {0x31, { "tgeu", OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::geu>, MIPS_II_Instr} },
    {0x32, { "tlt",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::lt>,  MIPS_II_Instr} },
    {0x33, { "tltu", OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::ltu>, MIPS_II_Instr} },
    {0x34, { "teq",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::eq>,  MIPS_II_Instr} },
    //        0x35 reserved
    {0x36, { "tne",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::ne>,   MIPS_II_Instr} },
    //        0x37 reserved

    // Doubleword shifts
    //key     name    operation     ms   source1       source2     destination       pointer                           mips version
    {0x38, { "dsll"   , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_sll<uint64>, MIPS_III_Instr} },
    {0x3A, { "dsrl"   , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_srl<uint64>, MIPS_III_Instr} },
    {0x3B, { "dsra"   , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::template execute_sra<uint64>, MIPS_III_Instr} },
    {0x3C, { "dsll32" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_dsll32,      MIPS_III_Instr} },
    {0x3E, { "dsrl32" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_dsrl32,      MIPS_III_Instr} },
    {0x3F, { "dsra32" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_dsra32,      MIPS_III_Instr} }
};

//unordered map for RI-instructions
template<typename RegisterUInt>
static const Table<RegisterUInt> isaMapRI =
{
// ********************** REGIMM INSTRUCTIONS *************************
    // Branches
    //key     name    operation   memsize    source1        source2      destination      pointer                                                       mips version
    {0x0,  { "bltz",  OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::ltz>, MIPS_I_Instr} },
    {0x1,  { "bgez",  OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::gez>, MIPS_I_Instr} },
    {0x2,  { "bltzl", OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::ltz>, MIPS_II_Instr} },
    {0x3,  { "bgezl", OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::gez>, MIPS_II_Instr} },

    // Traps
    //key     name    operation    ms  source1        source2      destination      pointer                                                       mips version
    {0x8,  { "tgei",  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::gei>,  MIPS_II_Instr} },
    {0x9,  { "tgeiu", OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::geiu>, MIPS_II_Instr} },
    {0xA,  { "tlti",  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::lti>,  MIPS_II_Instr} },
    {0xB,  { "tltiu", OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::ltiu>, MIPS_II_Instr} },
    {0xC,  { "teqi",  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::eqi>,  MIPS_II_Instr} },
    {0xE,  { "tnei",  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_trap<&BaseMIPSInstr<RegisterUInt>::nei>,  MIPS_II_Instr} },

    // Linking branches
    //key     name    operation   memsize     source1        source2      destination      pointer                                                             mips version
    {0x10, { "bltzal",  OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::RA, &BaseMIPSInstr<RegisterUInt>::template execute_branch_and_link<&BaseMIPSInstr<RegisterUInt>::ltz>, MIPS_I_Instr} },
    {0x11, { "bgezal",  OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::RA, &BaseMIPSInstr<RegisterUInt>::template execute_branch_and_link<&BaseMIPSInstr<RegisterUInt>::gez>, MIPS_I_Instr} },
    {0x12, { "bltzall", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::RA, &BaseMIPSInstr<RegisterUInt>::template execute_branch_and_link<&BaseMIPSInstr<RegisterUInt>::ltz>, MIPS_II_Instr} },
    {0x13, { "bgezall", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::RA, &BaseMIPSInstr<RegisterUInt>::template execute_branch_and_link<&BaseMIPSInstr<RegisterUInt>::gez>, MIPS_II_Instr} }
};

//unordered map for I-instructions and J-instructions
template<typename RegisterUInt>
static const Table<RegisterUInt> isaMapIJ =
{
    // ********************* I and J INSTRUCTIONS *************************
    // Direct jumps
    //key     name    operation  memsize    source1      source2      destination      pointer                          mips version
    {0x2, { "j",   OUT_J_JUMP, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_j,     MIPS_I_Instr } },
    {0x3, { "jal", OUT_J_JUMP, 0, RegType::ZERO, RegType::ZERO, RegType::RA,
                                &BaseMIPSInstr<RegisterUInt>::template execute_jump_and_link<&BaseMIPSInstr<RegisterUInt>::execute_j>,   MIPS_I_Instr } },

    // Branches
    //key     name    operation  memsize    source1      source2      destination      pointer                                                     mips version
    {0x4, { "beq",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::eq>,  MIPS_I_Instr} },
    {0x5, { "bne",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::ne>,  MIPS_I_Instr} },
    {0x6, { "blez", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::lez>, MIPS_I_Instr} },
    {0x7, { "bgtz", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::ZERO, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::gtz>, MIPS_I_Instr} },

    // Addition/Subtraction
    //key     name    operation  memsize    source1      source2   destination      pointer                                     mips version
    {0x8, { "addi",  OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::template execute_addition_imm<int32>,  MIPS_I_Instr} },
    {0x9, { "addiu", OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::template execute_addition_imm<uint32>, MIPS_I_Instr} },

    // Logical operations
    //key     name    operation  memsize    source1      source2      destination      pointer                                                     mips version
    {0xA, { "slti",  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::template execute_set<&BaseMIPSInstr<RegisterUInt>::lti>,  MIPS_I_Instr} },
    {0xB, { "sltiu", OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::template execute_set<&BaseMIPSInstr<RegisterUInt>::ltiu>, MIPS_I_Instr} },
    {0xC, { "andi",  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::execute_andi,   MIPS_I_Instr} },
    {0xD, { "ori",   OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::execute_ori,    MIPS_I_Instr} },
    {0xE, { "xori",  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::execute_xori,   MIPS_I_Instr} },
    {0xF, { "lui",   OUT_I_CONST,  0, RegType::ZERO, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::execute_lui,    MIPS_I_Instr} },

    // 0x10 - 0x13 coprocessor operations

    // Likely branches (MIPS II)
    //key     name    operation   memsize    source1      source2      destination      pointer                                                           mips version
    {0x14, { "beql",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::eq>,  MIPS_II_Instr} },
    {0x15, { "bnel",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::ne>,  MIPS_II_Instr} },
    {0x16, { "blezl", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::RT,   RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::lez>, MIPS_II_Instr} },
    {0x17, { "bgtzl", OUT_RI_BRANCH_0, 0, RegType::RS, RegType::RT,   RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::template execute_branch<&BaseMIPSInstr<RegisterUInt>::gtz>, MIPS_II_Instr} },

    // Doubleword unaligned loads
    //key     name    operation  memsize    source1      source2      destination      pointer                         mips version
    {0x1A, { "ldl",  OUT_I_LOAD, 8, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr, MIPS_III_Instr} },
    {0x1B, { "ldr",  OUT_I_LOAD, 8, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr, MIPS_III_Instr} },

    // Doubleword addition
    //key     name    operation  memsize    source1      source2     destination      pointer                                        mips version
    {0x18, { "daddi",  OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::template execute_addition_imm<int64>,  MIPS_III_Instr} },
    {0x19, { "daddiu", OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::template execute_addition_imm<uint64>, MIPS_III_Instr} },

    // Loads
    //key     name    operation     memsize    source1      source2      destination      pointer                                 mips version
    {0x20, { "lb",  OUT_I_LOAD,         1, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr,         MIPS_I_Instr} },
    {0x21, { "lh",  OUT_I_LOAD,         2, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr_aligned, MIPS_I_Instr} },
    {0x22, { "lwl", OUT_I_PARTIAL_LOAD, 4, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr_left32,  MIPS_I_Instr} },
    {0x23, { "lw",  OUT_I_LOAD,         4, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr_aligned, MIPS_I_Instr} },
    {0x24, { "lbu", OUT_I_LOADU,        1, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr,         MIPS_I_Instr} },
    {0x25, { "lhu", OUT_I_LOADU,        2, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr_aligned, MIPS_I_Instr} },
    {0x26, { "lwr", OUT_I_PARTIAL_LOAD, 4, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr_right32, MIPS_I_Instr} },
    {0x27, { "lwu", OUT_I_LOADU,        4, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr_aligned, MIPS_I_Instr} },

    // Stores
    //key     name    operation  memsize    source1      source2      destination      pointer                                         mips version
    {0x28, { "sb",  OUT_I_STORE, 1, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::calculate_store_addr,         MIPS_I_Instr} },
    {0x29, { "sh",  OUT_I_STORE, 2, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::calculate_store_addr,         MIPS_I_Instr} },
    {0x2A, { "swl", OUT_I_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::calculate_store_addr_left32,  MIPS_I_Instr} },
    {0x2B, { "sw",  OUT_I_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::calculate_store_addr_aligned, MIPS_I_Instr} },
    {0x2C, { "sdl", OUT_I_STORE, 8, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::calculate_store_addr,         MIPS_III_Instr} },
    {0x2D, { "sdr", OUT_I_STORE, 8, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::calculate_store_addr,         MIPS_III_Instr} },
    {0x2E, { "swr", OUT_I_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::calculate_store_addr_right32, MIPS_I_Instr} },
    //       0x2F   cache

    // Advanced loads and stores
    {0x30, { "ll",  OUT_I_LOAD,   2, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr,  MIPS_I_Instr} },

    {0x37, { "ld",  OUT_I_LOAD,   8, RegType::RS, RegType::ZERO, RegType::RT, &BaseMIPSInstr<RegisterUInt>::calculate_load_addr,  MIPS_III_Instr} },
    {0x38, { "sc",  OUT_I_STORE,  2, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::calculate_store_addr, MIPS_I_Instr} },

    {0x3F, { "sd",  OUT_I_STORE,  8, RegType::RS, RegType::RT, RegType::ZERO, &BaseMIPSInstr<RegisterUInt>::calculate_store_addr, MIPS_III_Instr} }
};

template<typename RegisterUInt>
static const Table<RegisterUInt> isaMapMIPS32 =
{
    // ********************* MIPS32 INSTRUCTIONS *************************
    // Advanced multiplication
    //key     name    operation  memsize    source1      source2      destination      pointer                                         mips version
    {0x00, { "madd",  OUT_R_ACCUM,   0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_multiplication<int32>,  MIPS_32_Instr} },
    {0x01, { "maddu", OUT_R_ACCUM,   0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_multiplication<uint32>, MIPS_32_Instr} },
    {0x02, { "mul",   OUT_R_ARITHM,  0, RegType::RS, RegType::RT, RegType::RD,    &BaseMIPSInstr<RegisterUInt>::template execute_multiplication<int32>,  MIPS_32_Instr} },
    {0x04, { "msub",  OUT_R_SUBTR,   0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_multiplication<int32>,  MIPS_32_Instr} },
    {0x05, { "msubu", OUT_R_SUBTR,   0, RegType::RS, RegType::RT, RegType::HI_LO, &BaseMIPSInstr<RegisterUInt>::template execute_multiplication<uint32>, MIPS_32_Instr} },

    // Count leading zeroes/ones
    //key     name    operation  memsize    source1      source2      destination      pointer                       mips version
    {0x20, { "clz",   OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_clz,     MIPS_32_Instr} },
    {0x21, { "clo",   OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_clo,     MIPS_32_Instr} },
    {0x24, { "dclz",  OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_dclz,    MIPS_64_Instr} },
    {0x25, { "dclo",  OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, &BaseMIPSInstr<RegisterUInt>::execute_dclo,    MIPS_64_Instr} }
};

template<typename RegisterUInt>
BaseMIPSInstr<RegisterUInt>::BaseMIPSInstr( MIPSVersion version, uint32 bytes, Addr PC) :
    raw( bytes),
    new_PC( PC + 4),
    PC( PC)
{
    bool valid = false;
    auto it = isaMapRI<RegisterUInt>.cbegin();
    MIPSInstrDecoder instr( raw);

    switch ( instr.opcode)
    {
        case 0x0: // R instruction
            it = isaMapR<RegisterUInt>.find( instr.funct);
            valid = ( it != isaMapR<RegisterUInt>.end());
            break;

        case 0x1: // RegIMM instruction
            it = isaMapRI<RegisterUInt>.find( instr.rt);
            valid = ( it != isaMapRI<RegisterUInt>.end());
            break;

        case 0x1C: // MIPS32 instruction
            it = isaMapMIPS32<RegisterUInt>.find( instr.funct);
            valid = ( it != isaMapMIPS32<RegisterUInt>.end());
            break;

        default: // I and J instructions
            it = isaMapIJ<RegisterUInt>.find( instr.opcode);
            valid = ( it != isaMapIJ<RegisterUInt>.end());
            break;
    }

    if ( valid)
    {
        init( it->second, instr, version);
    }
    else {
        std::ostringstream oss;
        if ( PC != 0)
            oss << std::hex << "0x" << PC << ": ";
        oss << std::hex << std::setfill( '0')
            << "0x" << std::setw( 8) << raw << '\t' << "Unknown";
        disasm = oss.str();
    }
}

template<typename M>
auto find_entry( const M& map, std::string_view name)
{
    return std::find_if( map.begin(), map.end(), [name]( const auto& e) {
        return e.second.name == name;
    });
}

template<typename RegisterUInt>
BaseMIPSInstr<RegisterUInt>::BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, Addr PC)
    : raw( 0)
    , new_PC( PC + 4)
    , PC( PC)
{
    auto it = find_entry( isaMapR<RegisterUInt>,      str_opcode);
    if ( it == isaMapR<RegisterUInt>.end())
        it =  find_entry( isaMapRI<RegisterUInt>,     str_opcode);
    if ( it == isaMapRI<RegisterUInt>.end())
        it =  find_entry( isaMapMIPS32<RegisterUInt>, str_opcode);
    if ( it == isaMapMIPS32<RegisterUInt>.end())
        it =  find_entry( isaMapIJ<RegisterUInt>,     str_opcode);

    if ( str_opcode == "nop")
    {
        init (isaMapR<RegisterUInt>.find( 0)->second, MIPSInstrDecoder( 0), version);
        disasm = "nop ";
    }
    else if ( it == isaMapIJ<RegisterUInt>.end())
    {
        std::ostringstream oss;
        if ( PC != 0)
            oss << std::hex << "0x" << PC << ": ";
        oss << str_opcode << '\t' << "Unknown";
        disasm = oss.str();
    }
    else {
        init( it->second, MIPSInstrDecoder( 0), version);
    }
}

template<typename RegisterUInt>
void BaseMIPSInstr<RegisterUInt>::init( const MIPSTableEntry<RegisterUInt>& entry,
                                        const MIPSInstrDecoder& instr,
                                        MIPSVersion version)
{
    operation = entry.operation;
    mem_size  = entry.mem_size;
    function  = entry.versions.is_supported(version)
        ? entry.function
        : &BaseMIPSInstr<RegisterUInt>::execute_unknown;

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
    if ( raw == 0x0ul)
        disasm = "nop ";
    else
        disasm = oss.str();
}

template<typename RegisterUInt>
void BaseMIPSInstr<RegisterUInt>::execute_unknown()
{
    std::ostringstream oss;
    oss << *this;
    throw UnknownMIPSInstruction( oss.str());
}

template<typename RegisterUInt>
void BaseMIPSInstr<RegisterUInt>::execute()
{
    (this->*function)();
    complete = true;
}

template<typename RegisterUInt>
void BaseMIPSInstr<RegisterUInt>::set_v_dst( RegisterUInt value)
{
    memory_complete = true;
    if ( operation == OUT_I_LOAD || is_partial_load())
    {
        switch ( get_mem_size())
        {
            case 1: v_dst = narrow_cast<int8>( value); break;
            case 2: v_dst = narrow_cast<int16>( value); break;
            case 4: v_dst = narrow_cast<int32>( value); break;
            case 8: v_dst = value; break;
            default: assert( false);
        }
    }
    else if ( operation == OUT_I_LOADU)
    {
        v_dst = value;
    }
    else
    {
        assert( false);
    }
}
template<typename RegisterUInt>
std::string BaseMIPSInstr<RegisterUInt>::string_dump() const
{
    std::ostringstream oss;
    dump( oss);
    return oss.str();
}

template<typename RegisterUInt>
std::ostream& BaseMIPSInstr<RegisterUInt>::dump( std::ostream& out) const
{
    out << "{" << sequence_id << "}\t";
    out << disasm;
    if ( !dst.is_zero() && (is_load() ? memory_complete : complete) && get_mask() != 0)
    {
        out << "\t [ $" << dst << " = 0x" << std::hex << (v_dst & mask);
        if ( !dst2.is_zero())
            out << ", $" << dst2 << " = 0x" << v_dst2;
        out << " ]";
    }
    if ( trap != Trap::NO_TRAP)
        out << "\t trap";

    out << std::dec;
    return out;
}

template class BaseMIPSInstr<uint32>;
template class BaseMIPSInstr<uint64>;
