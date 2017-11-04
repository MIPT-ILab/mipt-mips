/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#include <iostream>
#include <sstream>
#include <iomanip>

#include "mips_instr.h"

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    { "###", 0xFF, FORMAT_UNKNOWN, OUT_UNKNOWN, 0, &FuncInstr::execute_unknown, 1},

    // **************** R INSTRUCTIONS ****************
    // Constant shifts
    // name funct   format    operation memsize           pointer
    { "sll", 0x0, FORMAT_R, OUT_R_SHAMT, 0, &FuncInstr::execute_sll, 1},
    //       0x1 movci
    { "srl", 0x2, FORMAT_R, OUT_R_SHAMT, 0, &FuncInstr::execute_srl, 1},
    { "sra", 0x3, FORMAT_R, OUT_R_SHAMT, 0, &FuncInstr::execute_sra, 1},

    // Variable shifts
    // name  funct   format    operation  memsize           pointer
    { "sllv", 0x4, FORMAT_R, OUT_R_SHIFT, 0, &FuncInstr::execute_sllv, 1},
    //        0x5 reserved
    { "srlv", 0x6, FORMAT_R, OUT_R_SHIFT, 0, &FuncInstr::execute_srlv, 1},
    { "srav", 0x7, FORMAT_R, OUT_R_SHIFT, 0, &FuncInstr::execute_srav, 1},

    // Indirect branches
    // name  funct  format    operation     memsize           pointer
    { "jr",   0x8, FORMAT_R, OUT_R_JUMP,      0, &FuncInstr::execute_jr,   1},
    { "jalr", 0x9, FORMAT_R, OUT_R_JUMP_LINK, 0, &FuncInstr::execute_jalr, 1},

    // Conditional moves (MIPS IV)
    // name  funct  format    operation  memsize          pointer
    { "movz", 0xA, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_movz, 4},
    { "movn", 0xB, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_movn, 4},

    // System calls
    // name    funct format    operation     memsize           pointer
    { "syscall",0xC, FORMAT_R, OUT_R_SPECIAL, 0, &FuncInstr::execute_syscall, 1},
    { "break",  0xD, FORMAT_R, OUT_R_SPECIAL, 0, &FuncInstr::execute_break,   1},
    //          0xE reserved
    //          0xF SYNC    

    // HI/LO manipulations
    // name   funct   format    operation     memsize           pointer
    { "mfhi", 0x10,  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mfhi, 1},
    { "mthi", 0x11,  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mthi, 1},
    { "mflo", 0x12,  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mflo, 1},
    { "mtlo", 0x13,  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mtlo, 1},

    // 0x14 - 0x17 double width shifts

    // Multiplication/Division
    // name    funct    format    operation  memsize           pointer
    { "mult",  0x18,  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mult,  1},
    { "multu", 0x19,  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_multu, 1},
    { "div",   0x1A,  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_div,   1},
    { "divu",  0x1B,  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_divu,  1},

    // 0x1C - 0x1F double width multiplication/division

    // Addition/Subtraction
    // name    funct    format    operation  memsize           pointer
    { "add",  0x20, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_add,  1},
    { "addu", 0x21, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_addu, 1},
    { "sub",  0x22, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_sub,  1},
    { "subu", 0x23, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_subu, 1},

    // Logical operations
    // name    funct    format    operation  memsize           pointer
    { "and",  0x24, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_and,  1},
    { "or",   0x25, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_or,   1},
    { "xor",  0x26, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_xor,  1},
    { "nor",  0x27, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_nor,  1},
    //        0x28 reserved
    //        0x29 reserved
    { "slt",  0x2A, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_slt,  1},
    { "sltu", 0x2B, FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_sltu, 1},

    // 0x2C - 0x2F double width addition/substraction
    
    // Conditional traps (MIPS II)
    // name  funct    format operation  memsize           pointer
    { "tge",  0x30, FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tge,  2},
    { "tgeu", 0x31, FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tgeu, 2},
    { "tlt",  0x32, FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tlt,  2},
    { "tltu", 0x33, FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tltu, 2},
    { "teq",  0x34, FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_teq,  2},
    //        0x35 reserved
    { "tne",  0x36, FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tne,  2},
    //        0x37 reserved

    // 0x38 - 0x3F double width shifts

    // ********************** REGIMM INSTRUCTIONS *************************

    // ********************* I and J INSTRUCTIONS *************************
    // Branches
    // name opcode  format    operation     memsize           pointer
    { "j",    0x2, FORMAT_J, OUT_J_JUMP,      0, &FuncInstr::execute_j,    1},
    { "jal",  0x3, FORMAT_J, OUT_J_JUMP_LINK, 0, &FuncInstr::execute_jal,  1},
    { "beq",  0x4, FORMAT_I, OUT_I_BRANCH,    0, &FuncInstr::execute_beq,  1},
    { "bne",  0x5, FORMAT_I, OUT_I_BRANCH,    0, &FuncInstr::execute_bne,  1},
    { "blez", 0x6, FORMAT_I, OUT_I_BRANCH_0,  0, &FuncInstr::execute_blez, 1},
    { "bgtz", 0x7, FORMAT_I, OUT_I_BRANCH_0,  0, &FuncInstr::execute_bgtz, 1},

    // Addition/Subtraction
    // name   opcode  format    operation  memsize           pointer
    { "addi",  0x8,   FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_addi,  1},
    { "addiu", 0x9,   FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_addiu, 1},

    // Logical operations
    // name   opcode    format    operation  memsize           pointer
    { "slti",  0xA,   FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_slti,  1},
    { "sltiu", 0xB,   FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_sltiu, 1},
    { "andi",  0xC,   FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_andi,  1},
    { "ori",   0xD,   FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_ori,   1},
    { "xori",  0xE,   FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_xori,  1},
    { "lui",   0xF,   FORMAT_I, OUT_I_CONST,  0, &FuncInstr::execute_lui,   1},

    // 0x10 - 0x13 coprocessor operations 

    // Likely branches (MIPS II)
    // name  opcode    format    operation memsize           pointer
    { "beql",  0x14,  FORMAT_I, OUT_I_BRANCH,   0, &FuncInstr::execute_beq,  2},
    { "bnel",  0x15,  FORMAT_I, OUT_I_BRANCH,   0, &FuncInstr::execute_bne,  2},
    { "blezl", 0x16,  FORMAT_I, OUT_I_BRANCH_0, 0, &FuncInstr::execute_blez, 2},
    { "bgtzl", 0x17,  FORMAT_I, OUT_I_BRANCH_0, 0, &FuncInstr::execute_bgtz, 2},

    // 0x18 - 0x19 double width addition
    // 0x1A - 0x1B load double word left/right

    // Loads
    // name opcode    format    operation memsize           pointer
    { "lb",  0x20,  FORMAT_I, OUT_I_LOAD,  1, &FuncInstr::calculate_load_addr, 1},
    { "lh",  0x21,  FORMAT_I, OUT_I_LOAD,  2, &FuncInstr::calculate_load_addr, 1},
    { "lwl", 0x22,  FORMAT_I, OUT_I_LOADL, 4, &FuncInstr::calculate_load_addr, 1},
    { "lw",  0x23,  FORMAT_I, OUT_I_LOAD,  4, &FuncInstr::calculate_load_addr, 1},
    { "lbu", 0x24,  FORMAT_I, OUT_I_LOADU, 1, &FuncInstr::calculate_load_addr, 1},
    { "lhu", 0x25,  FORMAT_I, OUT_I_LOADU, 2, &FuncInstr::calculate_load_addr, 1},
    { "lwr", 0x26,  FORMAT_I, OUT_I_LOADR, 4, &FuncInstr::calculate_load_addr, 1},
    { "lwu", 0x27,  FORMAT_I, OUT_I_LOADU, 4, &FuncInstr::calculate_load_addr, 1},

    // Store
    // name opcode    format    operation memsize           pointer
    { "sb",  0x28,  FORMAT_I, OUT_I_STORE,  1, &FuncInstr::calculate_store_addr, 1},
    { "sh",  0x29,  FORMAT_I, OUT_I_STORE,  2, &FuncInstr::calculate_store_addr, 1},
    { "swl", 0x2A,  FORMAT_I, OUT_I_STOREL, 4, &FuncInstr::calculate_store_addr, 1},
    { "sw",  0x2B,  FORMAT_I, OUT_I_STORE,  4, &FuncInstr::calculate_store_addr, 1},
    //       0x2C   store double word left
    //       0x2D   store double word right
    { "swr", 0x2E,  FORMAT_I, OUT_I_STORER, 4, &FuncInstr::calculate_store_addr, 1}
    //       0x2F   coprocessor
    
    // 0x30 - 0x3F atomic load/stores
};

std::array<string_view, REG_NUM_MAX> FuncInstr::regTable =
{{
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9",
    "k0", "k1",
    "gp",
    "sp",
    "fp",
    "ra"
}};

string_view FuncInstr::regTableName(RegNum reg) {


    return regTable.at(static_cast<size_t>( reg));
}

FuncInstr::FuncInstr( uint32 bytes, Addr PC,
                      bool predicted_taken,
                      Addr predicted_target) :
    instr( bytes),
    predicted_taken( predicted_taken),
    predicted_target( predicted_target),
    PC( PC)
{
    initFormat();
    switch ( format)
    {
        case FORMAT_R:
            initR();
            break;
        case FORMAT_I:
            initI();
            break;
        case FORMAT_J:
            initJ();
            break;
        case FORMAT_UNKNOWN:
            initUnknown();
            break;
    }
    new_PC = PC + 4;
}

void FuncInstr::initFormat()
{
    bool is_R = ( instr.asR.opcode == 0x0);
    uint8 ident = is_R ? instr.asR.funct : instr.asR.opcode;

    for ( const auto& entry : isaTable)
    {
        bool is_i_R = ( entry.format == FORMAT_R);
        if ( entry.opcode == ident && ( is_i_R == is_R))
        {
            format    = entry.format;
            operation = entry.operation;
            mem_size  = entry.mem_size;
            name      = entry.name;
            function  = entry.function;
            return;
        }
    }
    initUnknown();
}

void FuncInstr::initR()
{
    std::ostringstream oss;
    if ( PC != 0)
        oss << std::hex << "0x" << PC << ": ";
    oss << name;
    switch ( operation)
    {
        case OUT_R_ARITHM:
            src2 = static_cast<RegNum>(instr.asR.rt);
            src1 = static_cast<RegNum>(instr.asR.rs);
            dst  = static_cast<RegNum>(instr.asR.rd);

            oss <<  " $" << regTableName(dst)
                << ", $" << regTableName(src1)
                << ", $" << regTableName(src2);
            break;
        case OUT_R_SHIFT:
            src2 = static_cast<RegNum>(instr.asR.rs);
            src1 = static_cast<RegNum>(instr.asR.rt);
            dst  = static_cast<RegNum>(instr.asR.rd);

            oss <<  " $" << regTableName(dst)
                << ", $" << regTableName(src1)
                << ", $" << regTableName(src2);
            break;

        case OUT_R_SHAMT:
            src1  = static_cast<RegNum>(instr.asR.rt);
            dst   = static_cast<RegNum>(instr.asR.rd);
            shamt = instr.asR.shamt;

            oss <<  " $" << regTableName(dst)
                << ", $" << regTableName(src1)
                <<  ", " << std::dec << shamt;
            break;
        case OUT_R_JUMP_LINK:
            src1  = static_cast<RegNum>(instr.asR.rs);
            dst   = static_cast<RegNum>(instr.asR.rd);
            oss <<  " $" << regTableName(dst)
                << ", $" << regTableName(src1);
            break;
        case OUT_R_JUMP:
            dst = REG_NUM_ZERO;
            src1  = static_cast<RegNum>(instr.asR.rs);
            oss << " $" << regTableName(src1);
            break;
        case OUT_R_TRAP:
            dst = REG_NUM_ZERO;
            src1 = static_cast<RegNum>(instr.asR.rs);
            src2 = static_cast<RegNum>(instr.asR.rt);

            oss <<  " $" << regTableName(src1)
                << ", $" << regTableName(src2);
            break;
        case OUT_R_SPECIAL:
            break;
        default:
            assert( false);
    }
    if ( instr.raw == 0x0ul)
        disasm = "nop ";
    else
        disasm = oss.str();
}


void FuncInstr::initI()
{
    v_imm = instr.asI.imm;

    std::ostringstream oss;
    if ( PC != 0)
        oss << std::hex << "0x" << PC << ": ";
    oss << name << " $";
    switch ( operation)
    {
        case OUT_I_ARITHM:
            src1 = static_cast<RegNum>(instr.asI.rs);
            dst  = static_cast<RegNum>(instr.asI.rt);

            oss << regTable[dst] << ", $"
                << regTable[src1] << ", "
                << std::hex << "0x" << v_imm << std::dec;

            break;
        case OUT_I_BRANCH:
            src1 = static_cast<RegNum>(instr.asI.rs);
            src2 = static_cast<RegNum>(instr.asI.rt);

            oss << regTable[src1] << ", $"
                << regTable[src2] << ", "
                << std::dec << static_cast<int16>(v_imm);
            break;
        case OUT_I_BRANCH_0:
            src1 = static_cast<RegNum>(instr.asI.rs);

            oss << regTable[src1] << ", "
                << std::dec << static_cast<int16>(v_imm);
            break;

        case OUT_I_CONST:
            dst  = static_cast<RegNum>(instr.asI.rt);

            oss << regTable[dst] << std::hex
                << ", 0x" << v_imm << std::dec;
            break;

        case OUT_I_LOAD:
        case OUT_I_LOADU:
        case OUT_I_LOADL:
        case OUT_I_LOADR:
            src1 = static_cast<RegNum>(instr.asI.rs);
            dst  = static_cast<RegNum>(instr.asI.rt);

            oss << regTable[dst] << ", 0x"
                << std::hex << v_imm
                << "($" << regTable[src1] << ")" << std::dec;
            break;

        case OUT_I_STORE:
        case OUT_I_STOREL:
        case OUT_I_STORER:
            src2 = static_cast<RegNum>(instr.asI.rt);
            src1 = static_cast<RegNum>(instr.asI.rs);
            dst  = REG_NUM_ZERO;

            oss << regTable[src2] << ", 0x"
                << std::hex << v_imm
                << "($" << regTable[src1] << ")" << std::dec;
            break;
        default:
            assert( false);
    }
    disasm = oss.str();
}

void FuncInstr::initJ()
{
    v_imm = instr.asJ.imm;

    std::ostringstream oss;
    if ( PC != 0)
        oss << std::hex << "0x" << PC << ": ";
    oss << name << " 0x"
        << std::hex << static_cast<uint16>(v_imm) << std::dec;

    if ( operation == OUT_J_JUMP_LINK)
        dst = REG_NUM_RA;
    else
        dst = REG_NUM_ZERO;

    disasm = oss.str();
}

void FuncInstr::initUnknown()
{
    std::ostringstream oss;
    if ( PC != 0)
        oss << std::hex << "0x" << PC << ": ";
    oss << std::hex << std::setfill( '0')
        << "0x" << std::setw( 8) << instr.raw << '\t' << "Unknown";
    disasm = oss.str();
}

void FuncInstr::execute_unknown()
{
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}

void FuncInstr::execute()
{
    (this->*function)();
    complete = true;

    if ( dst != REG_NUM_ZERO && !is_load())
    {
        std::ostringstream oss;
        oss << "\t [ $" << regTableName(dst)
            << " = 0x" << std::hex << v_dst << "]";
        disasm += oss.str();
    }
}

void FuncInstr::set_v_dst( uint32 value)
{
    if ( operation == OUT_I_LOAD)
    {
        switch ( get_mem_size())
        {
            case 1: v_dst = static_cast<int32>( static_cast<int8>( value)); break;
            case 2: v_dst = static_cast<int32>( static_cast<int16>( value)); break;
            case 4: v_dst = value; break;
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

    if ( dst != REG_NUM_ZERO)
    {
        std::ostringstream oss;
        oss << "\t [ $" << regTableName(dst)
            << " = 0x" << std::hex << v_dst << "]";
        disasm += oss.str();
    }
}

void FuncInstr::check_trap()
{
    if ( trap != TrapType::NO_TRAP)
    {
        std::ostringstream oss;
        oss << "\t trap";
        disasm += oss.str();
    }
}

