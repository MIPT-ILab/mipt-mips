/*
 * mips_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015-2018 MIPT-MIPS
 */

#include <iostream>
#include <sstream>
#include <iomanip>

#include "mips_instr.h"

//unordered map for R-instructions
template<typename RegisterUInt>
const typename MIPSInstr<RegisterUInt>::MapType MIPSInstr<RegisterUInt>::isaMapR =
{
    // **************** R INSTRUCTIONS ****************
    // Constant shifts
    //key     name    operation  ms  source1      source2      destination      pointer                              mips version
    {0x0, { "sll" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_sll<uint32>, 1} },
    //       0x1 movci
    {0x2, { "srl", OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_srl, 1} },
    {0x3, { "sra", OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_sra<uint32>, 1} },

    // Variable shifts
    //key     name    operation  ms  source1      source2      destination      pointer                       mips version
    {0x4, { "sllv", OUT_R_SHIFT, 0, RegType::RT, RegType::RS, RegType::RD, &MIPSInstr<RegisterUInt>::execute_sllv, 1} },
    //        0x5 reserved
    {0x6, { "srlv", OUT_R_SHIFT, 0, RegType::RT, RegType::RS, RegType::RD, &MIPSInstr<RegisterUInt>::execute_srlv<uint32>, 1} },
    {0x7, { "srav", OUT_R_SHIFT, 0, RegType::RT, RegType::RS, RegType::RD, &MIPSInstr<RegisterUInt>::execute_srav<uint32>, 1} },

    // Indirect branches
    //key     name    operation       ms  source1      source2      destination      pointer                           mips version
    {0x8, { "jr"  , OUT_R_JUMP,      0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_jr,   1} },
    {0x9, { "jalr", OUT_R_JUMP_LINK, 0, RegType::RS, RegType::ZERO, RegType::RD,   &MIPSInstr<RegisterUInt>::execute_jalr, 1} },

    // Conditional moves (MIPS IV)
    //key     name    operation       ms  source1  source2      destination      pointer                        mips version
    {0xA,  { "movz", OUT_R_CONDM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_movz, 4} },
    {0xB,  { "movn", OUT_R_CONDM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_movn, 4} },

    // System calls
    //key     name    operation       ms     source1       source2      destination      pointer                        mips version
    {0xC, { "syscall", OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_syscall, 1} },
    {0xD, { "break",   OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_break,   1} },
    //          0xE reserved
    //          0xF SYNC

    // HI/LO manipulations
    //key     name    operation  ms     source1       source2   destination      pointer                      mips version
    {0x10, { "mfhi", OUT_R_MFHI, 0, RegType::HI, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_move, 1} },
    {0x11, { "mthi", OUT_R_MTHI, 0, RegType::RS, RegType::ZERO, RegType::HI, &MIPSInstr<RegisterUInt>::execute_move, 1} },
    {0x12, { "mflo", OUT_R_MFLO, 0, RegType::LO, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_move, 1} },
    {0x13, { "mtlo", OUT_R_MTLO, 0, RegType::RS, RegType::ZERO, RegType::LO, &MIPSInstr<RegisterUInt>::execute_move, 1} },

    // Doubleword variable shifts
    //key     name    operation  ms     source1       source2   destination       pointer                             mips version
    {0x14, { "dsllv", OUT_R_SHIFT, 0, RegType::RT, RegType::RS, RegType::RD, &MIPSInstr<RegisterUInt>::execute_dsllv,        4} },
    {0x16, { "dsrlv", OUT_R_SHIFT, 0, RegType::RT, RegType::RS, RegType::RD, &MIPSInstr<RegisterUInt>::execute_srlv<uint64>, 4} },
    {0x17, { "dsrav", OUT_R_SHIFT, 0, RegType::RT, RegType::RS, RegType::RD, &MIPSInstr<RegisterUInt>::execute_srav<uint64>, 4} },

    // Multiplication/Division
    //key     name    operation  ms     source1       source2        destination       pointer                                       mips version
    {0x18, { "mult",  OUT_R_DIVMULT, 0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_multiplication<int32>,  1} },
    {0x19, { "multu", OUT_R_DIVMULT, 0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_multiplication<uint32>, 1} },
    {0x1A, { "div",   OUT_R_DIVMULT, 0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_division<int32>,        1} },
    {0x1B, { "divu",  OUT_R_DIVMULT, 0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_division<uint32>,       1} },
    {0x1C, { "dmult", OUT_R_DIVMULT, 0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_multiplication<int64>,  3} },
    {0x1D, { "dmultu",OUT_R_DIVMULT, 0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_multiplication<uint64>, 3} },
    {0x1E, { "ddiv",  OUT_R_DIVMULT, 0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_division<int64>,        3} },
    {0x1F, { "ddivu", OUT_R_DIVMULT, 0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_division<uint64>,       3} },

    // Addition/Subtraction
    //key     name    operation    ms   source1       source2   destination       pointer                             mips version
    {0x20, { "add",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_addition<int32>,     1} },
    {0x21, { "addu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_addition<uint32>,    1} },
    {0x22, { "sub",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_subtraction<int32>,  1} },
    {0x23, { "subu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_subtraction<uint32>, 1} },

    // Logical operations
    //key     name    operation   ms   source1       source2   destination       pointer                       mips version
    {0x24, { "and", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_and,  1} },
    {0x25, { "or",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_or,   1} },
    {0x26, { "xor", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_xor,  1} },
    {0x27, { "nor", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_nor,  1} },
    //        0x28 reserved
    //        0x29 reserved
    {0x2A, { "slt",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_set<&MIPSInstr<RegisterUInt>::lt>,  1} },
    {0x2B, { "sltu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_set<&MIPSInstr<RegisterUInt>::ltu>, 1} },

    // Doubleword addition/Subtraction
    //key     name    operation   ms   source1       source2     destination       pointer                                  mips version
    {0x2C, { "dadd",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_addition<int64>,     1} },
    {0x2D, { "daddu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_addition<uint64>,    1} },
    {0x2E, { "dsub",  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_subtraction<int64>,  3} },
    {0x2F, { "dsubu", OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, &MIPSInstr<RegisterUInt>::execute_subtraction<uint64>, 3} },

    // Conditional traps (MIPS II)
    //key     name    operation  ms   source1       source2     destination       pointer                                                    mips version
    {0x30, { "tge",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::ge>,  2} },
    {0x31, { "tgeu", OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::geu>, 2} },
    {0x32, { "tlt",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::lt>,  2} },
    {0x33, { "tltu", OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::ltu>, 2} },
    {0x34, { "teq",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::eq>,  2} },
    //        0x35 reserved
    {0x36, { "tne",  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::ne>,   2} },
    //        0x37 reserved

    // Doubleword shifts
    //key     name    operation       ms   source1       source2     destination       pointer                                    mips version
    {0x38, { "dsll"   , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_sll<uint64>, 4} },
    {0x3A, { "dsrl"   , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_dsrl,        4} },
    {0x3B, { "dsra"   , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_sra<uint64>, 4} },
    {0x3C, { "dsll32" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_dsll32,      4} },
    {0x3E, { "dsrl32" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_dsrl32,      4} },
    {0x3F, { "dsra32" , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_dsra32,      4} }
};

//unordered map for RI-instructions
template<typename RegisterUInt>
const typename MIPSInstr<RegisterUInt>::MapType MIPSInstr<RegisterUInt>::isaMapRI =
{
// ********************** REGIMM INSTRUCTIONS *************************
    // Branches
    //key     name    operation   memsize    source1        source2      destination      pointer                                                       mips version
    {0x0,  { "bltz",  OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::ltz>, 1} },
    {0x1,  { "bgez",  OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::gez>, 1} },
    {0x2,  { "bltzl", OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::ltz>, 2} },
    {0x3,  { "bgezl", OUT_RI_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::gez>, 2} },

    // Traps
    //key     name    operation memsize     source1        source2      destination      pointer                                                       mips version
    {0x8,  { "tgei",  OUT_RI_TRAP,      0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::gei>,  2} },
    {0x9,  { "tgeiu", OUT_RI_TRAP,      0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::geiu>, 2} },
    {0xA,  { "tlti",  OUT_RI_TRAP,      0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::lti>,  2} },
    {0xB,  { "tltiu", OUT_RI_TRAP,      0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::ltiu>, 2} },
    {0xC,  { "teqi",  OUT_RI_TRAP,      0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::eqi>,  2} },
    {0xE,  { "tnei",  OUT_RI_TRAP,      0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_trap<&MIPSInstr<RegisterUInt>::nei>,  2} },

    // Linking branches
    //key     name    operation           memsize     source1        source2      destination      pointer                                                        mips version
    {0x10, { "bltzal",  OUT_RI_BRANCH_LINK, 0, RegType::RS, RegType::ZERO, RegType::RA, &MIPSInstr<RegisterUInt>::execute_branch_and_link<&MIPSInstr<RegisterUInt>::ltz>, 1} },
    {0x11, { "bgezal",  OUT_RI_BRANCH_LINK, 0, RegType::RS, RegType::ZERO, RegType::RA, &MIPSInstr<RegisterUInt>::execute_branch_and_link<&MIPSInstr<RegisterUInt>::gez>, 1} },
    {0x12, { "bltzall", OUT_RI_BRANCH_LINK, 0, RegType::RS, RegType::ZERO, RegType::RA, &MIPSInstr<RegisterUInt>::execute_branch_and_link<&MIPSInstr<RegisterUInt>::ltz>, 2} },
    {0x13, { "bgezall", OUT_RI_BRANCH_LINK, 0, RegType::RS, RegType::ZERO, RegType::RA, &MIPSInstr<RegisterUInt>::execute_branch_and_link<&MIPSInstr<RegisterUInt>::gez>, 2} }
};

//unordered map for I-instructions and J-instructions
template<typename RegisterUInt>
const typename MIPSInstr<RegisterUInt>::MapType MIPSInstr<RegisterUInt>::isaMapIJ =
{
    // ********************* I and J INSTRUCTIONS *************************
    // Direct jumps
    //key     name    operation  memsize    source1      source2      destination      pointer                          mips version
    {0x2, { "j",   OUT_J_JUMP,      0, RegType::ZERO, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_j,    1 } },
    {0x3, { "jal", OUT_J_JUMP_LINK, 0, RegType::ZERO, RegType::ZERO, RegType::RA,   &MIPSInstr<RegisterUInt>::execute_jal,  1 } },

    // Branches
    //key     name    operation  memsize    source1      source2      destination      pointer                                                     mips version
    {0x4, { "beq",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::eq>,  1} },
    {0x5, { "bne",  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::ne>,  1} },
    {0x6, { "blez", OUT_I_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::lez>, 1} },
    {0x7, { "bgtz", OUT_I_BRANCH_0,  0, RegType::RS, RegType::ZERO, RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::gtz>, 1} },

    // Addition/Subtraction
    //key     name    operation  memsize    source1      source2   destination      pointer                                     mips version
    {0x8, { "addi",  OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_addition_imm<int32>,  1} },
    {0x9, { "addiu", OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_addition_imm<uint32>, 1} },

    // Logical operations
    //key     name    operation  memsize    source1      source2      destination      pointer                                                     mips version
    {0xA, { "slti",  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_set<&MIPSInstr<RegisterUInt>::lti>,  1} },
    {0xB, { "sltiu", OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_set<&MIPSInstr<RegisterUInt>::ltiu>, 1} },
    {0xC, { "andi",  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_andi,   1} },
    {0xD, { "ori",   OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_ori,    1} },
    {0xE, { "xori",  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_xori,   1} },
    {0xF, { "lui",   OUT_I_CONST,  0, RegType::ZERO, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_lui,    1} },

    // 0x10 - 0x13 coprocessor operations

    // Likely branches (MIPS II)
    //key     name    operation  memsize    source1      source2      destination      pointer                                                           mips version
    {0x14, { "beql",  OUT_I_BRANCH,   0, RegType::RS, RegType::RT,   RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::eq>,  2} },
    {0x15, { "bnel",  OUT_I_BRANCH,   0, RegType::RS, RegType::RT,   RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::ne>,  2} },
    {0x16, { "blezl", OUT_I_BRANCH_0, 0, RegType::RS, RegType::RT,   RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::lez>, 2} },
    {0x17, { "bgtzl", OUT_I_BRANCH_0, 0, RegType::RS, RegType::RT,   RegType::ZERO, &MIPSInstr<RegisterUInt>::execute_branch<&MIPSInstr<RegisterUInt>::gtz>, 2} },

    // Doubleword unaligned loads
    //key     name    operation  memsize    source1      source2      destination      pointer                         mips version
    {0x1A, { "ldl",  OUT_I_LOAD, 8, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr, 3} },
    {0x1B, { "ldr",  OUT_I_LOAD, 8, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr, 3} },

    // Doubleword addition 
    //key     name    operation  memsize    source1      source2     destination      pointer                                        mips version
    {0x18, { "daddi",  OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_addition_imm<int64>,  1} },
    {0x19, { "daddiu", OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::execute_addition_imm<uint64>, 1} },

    // Loads
    //key     name    operation  memsize    source1      source2      destination      pointer                                         mips version
    {0x20, { "lb",  OUT_I_LOAD,         1, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr,         1} },
    {0x21, { "lh",  OUT_I_LOAD,         2, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr_aligned, 1} },
    {0x22, { "lwl", OUT_I_PARTIAL_LOAD, 4, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr_left,    1} },
    {0x23, { "lw",  OUT_I_LOAD,         4, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr_aligned, 1} },
    {0x24, { "lbu", OUT_I_LOADU,        1, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr,         1} },
    {0x25, { "lhu", OUT_I_LOADU,        2, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr_aligned, 1} },
    {0x26, { "lwr", OUT_I_PARTIAL_LOAD, 4, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr_right,   1} },
    {0x27, { "lwu", OUT_I_LOADU,        4, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr_aligned, 1} },

    // Stores
    //key     name    operation  memsize    source1      source2      destination      pointer                                         mips version
    {0x28, { "sb",  OUT_I_STORE,         1, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::calculate_store_addr,         1} },
    {0x29, { "sh",  OUT_I_STORE,         2, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::calculate_store_addr,         1} },
    {0x2A, { "swl", OUT_I_PARTIAL_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::calculate_store_addr_left,    1} },
    {0x2B, { "sw",  OUT_I_STORE,         4, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::calculate_store_addr_aligned, 1} },
    {0x2C, { "sdl", OUT_I_PARTIAL_STORE, 8, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::calculate_store_addr,         3} },
    {0x2D, { "sdr", OUT_I_PARTIAL_STORE, 8, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::calculate_store_addr,         3} },
    {0x2E, { "swr", OUT_I_PARTIAL_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::calculate_store_addr_right,   1} },
    //       0x2F   cache

    // Advanced loads and stores
    {0x30, { "ll",  OUT_I_LOAD,   2, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr,  1} },

    {0x37, { "ld",  OUT_I_LOAD,   8, RegType::RS, RegType::ZERO, RegType::RT, &MIPSInstr<RegisterUInt>::calculate_load_addr,  3} },
    {0x38, { "sc",  OUT_I_STORE,  2, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::calculate_store_addr, 1} },

    {0x3F, { "sd",  OUT_I_STORE,  8, RegType::RS, RegType::RT, RegType::ZERO, &MIPSInstr<RegisterUInt>::calculate_store_addr, 3} }
};

template<typename RegisterUInt>
const typename MIPSInstr<RegisterUInt>::MapType MIPSInstr<RegisterUInt>::isaMapMIPS32 =
{
    // ********************* MIPS32 INSTRUCTIONS *************************
    // Advanced multiplication
    //key     name    operation  memsize    source1      source2      destination      pointer                                         mips version
    {0x00, { "madd",  OUT_R_ACCUM,   0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_multiplication<int32>,  32} },
    {0x01, { "maddu", OUT_R_ACCUM,   0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_multiplication<uint32>, 32} },
    {0x02, { "mul",   OUT_R_ARITHM,  0, RegType::RS, RegType::RT, RegType::RD,    &MIPSInstr<RegisterUInt>::execute_multiplication<int32>,  32} },
    {0x04, { "msub",  OUT_R_SUBTR,   0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_multiplication<int32>,  32} },
    {0x05, { "msubu", OUT_R_SUBTR,   0, RegType::RS, RegType::RT, RegType::HI_LO, &MIPSInstr<RegisterUInt>::execute_multiplication<uint32>, 32} },

    // Count leading zeroes/ones 
    //key     name    operation  memsize    source1      source2      destination      pointer                       mips version
    {0x20, { "clz",   OUT_SP2_COUNT, 0, RegType::RS, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_clz,     32} },
    {0x21, { "clo",   OUT_SP2_COUNT, 0, RegType::RS, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_clo,     32} },
    {0x24, { "dclz",  OUT_SP2_COUNT, 0, RegType::RS, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_dclz,    64} },
    {0x25, { "dclo",  OUT_SP2_COUNT, 0, RegType::RS, RegType::ZERO, RegType::RD, &MIPSInstr<RegisterUInt>::execute_dclo,    64} }
};

template<typename RegisterUInt>
MIPSInstr<RegisterUInt>::MIPSInstr( uint32 bytes, Addr PC) :
    instr( bytes),
    new_PC( PC + 4),
    PC( PC)
{
    bool valid = false;
    auto it = isaMapRI.cbegin();

    switch ( instr.asR.opcode)
    {
        case 0x0: // R instruction
            it = isaMapR.find( instr.asR.funct);
            valid = ( it != isaMapR.end());
            break;

        case 0x1: // RegIMM instruction
            it = isaMapRI.find( instr.asI.rt);
            valid = ( it != isaMapRI.end());
            break;

        case 0x1C: // MIPS32 instruction
            it = isaMapMIPS32.find( instr.asR.funct);
            valid = ( it != isaMapMIPS32.end());
            break;

        default: // I and J instructions
            it = isaMapIJ.find( instr.asR.opcode);
            valid = ( it != isaMapIJ.end());
            break;
    }

    if ( valid)
    {
        init( it->second);
    }
    else {
        std::ostringstream oss;
        if ( PC != 0)
            oss << std::hex << "0x" << PC << ": ";
        oss << std::hex << std::setfill( '0')
            << "0x" << std::setw( 8) << instr.raw << '\t' << "Unknown";
        disasm = oss.str();
    }
}

template<typename RegisterUInt>
MIPSRegister MIPSInstr<RegisterUInt>::get_register( RegType type) const
{
    switch ( type) {
    case RegType::HI:    return MIPSRegister::mips_hi;
    case RegType::LO:    return MIPSRegister::mips_lo;
    case RegType::HI_LO: return MIPSRegister::mips_lo;
    case RegType::ZERO:  return MIPSRegister::zero;
    case RegType::RA:    return MIPSRegister::return_address;
    case RegType::RS:    return MIPSRegister( instr.asR.rs);
    case RegType::RT:    return MIPSRegister( instr.asR.rt);
    case RegType::RD:    return MIPSRegister( instr.asR.rd);
    default: assert(0);  return MIPSRegister::zero;
    }
}

template<typename RegisterUInt>
void MIPSInstr<RegisterUInt>::init( const MIPSInstr<RegisterUInt>::ISAEntry& entry)
{
    operation = entry.operation;
    mem_size  = entry.mem_size;
    function  = entry.function;
    shamt     = instr.asR.shamt;

    src1 = get_register( entry.src1);
    src2 = get_register( entry.src2);
    dst  = get_register( entry.dst);
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
            v_imm = instr.asI.imm;

            oss << " $" << dst << ", $"
                << src1 << ", "
                << std::hex << "0x" << v_imm << std::dec;
            break;
        case OUT_I_BRANCH:
            v_imm = instr.asI.imm;

            oss << " $" << src1 << ", $"
                << src2 << ", "
                << std::dec << static_cast<int16>(v_imm);
            break;
        case OUT_RI_BRANCH_0:
        case OUT_I_BRANCH_0:
        case OUT_RI_BRANCH_LINK:
            v_imm = instr.asI.imm;
            oss << " $" << src1 << ", "
                << std::dec << static_cast<int16>(v_imm);
            break;
        case OUT_RI_TRAP:
            v_imm = instr.asI.imm;
            oss << " $" << src1 << ", 0x"
                << std::hex << static_cast<int16>(v_imm) << std::dec;
            break;
        case OUT_I_CONST:
            v_imm = instr.asI.imm;

            oss << " $" << dst << std::hex
                << ", 0x" << v_imm << std::dec;
            break;

        case OUT_I_LOAD:
        case OUT_I_LOADU:
        case OUT_I_PARTIAL_LOAD:
            v_imm = instr.asI.imm;

            oss << " $" << dst << ", 0x"
                << std::hex << v_imm
                << "($" << src1 << ")" << std::dec;
            break;

        case OUT_I_STORE:
        case OUT_I_PARTIAL_STORE:
            v_imm = instr.asI.imm;

            oss << " $" << src2 << ", 0x"
                << std::hex << v_imm
                << "($" << src1 << ")" << std::dec;
            break;
        case OUT_J_JUMP_LINK:
            v_imm = instr.asJ.imm;
            oss << " 0x"
                << std::hex << static_cast<uint16>(v_imm) << std::dec;
            break;
        case OUT_J_JUMP:
            v_imm = instr.asJ.imm;
            oss << " 0x"
                << std::hex << static_cast<uint16>(v_imm) << std::dec;
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
    if ( instr.raw == 0x0ul)
        disasm = "nop ";
    else
        disasm = oss.str();
}

template<typename RegisterUInt>
void MIPSInstr<RegisterUInt>::execute_unknown()
{
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}

template<typename RegisterUInt>
void MIPSInstr<RegisterUInt>::execute()
{
    (this->*function)();
    complete = true;

    if ( !dst.is_zero() && !is_load() && get_mask() != 0)
    {
        std::ostringstream oss;
        oss << "\t [ $" << dst << " = 0x" << std::hex << (v_dst & mask);
        if ( !dst2.is_zero())
            oss << ", $" << dst2 << " = 0x" << v_dst2;
        oss << " ]";
        disasm += oss.str();
    }
}

template<typename RegisterUInt>
void MIPSInstr<RegisterUInt>::set_v_dst( RegisterUInt value)
{
    if ( operation == OUT_I_LOAD || is_partial_load())
    {
        switch ( get_mem_size())
        {
            case 1: v_dst = static_cast<int8>( value); break;
            case 2: v_dst = static_cast<int16>( value); break;
            case 4: v_dst = static_cast<int32>( value); break;
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

    if ( !dst.is_zero())
    {
        std::ostringstream oss;
        oss << "\t [ $" << dst
            << " = 0x" << std::hex << v_dst;

        if (has_zero(get_mask()))
            oss << ", mask = 0x" << std::hex << mask;
        oss << " ]";
        disasm += oss.str();
    }
}

template<typename RegisterUInt>
void MIPSInstr<RegisterUInt>::check_trap()
{
    if ( trap != TrapType::NO_TRAP)
    {
        std::ostringstream oss;
        oss << "\t trap";
        disasm += oss.str();
    }
}

template class MIPSInstr<uint32>;
template class MIPSInstr<uint64>;
