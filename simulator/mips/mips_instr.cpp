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

//unordered map for R-instructions
const std::unordered_map <uint8, FuncInstr::ISAEntry> FuncInstr::isaMapR =
{
    {0xFF, {"###", FORMAT_UNKNOWN, OUT_UNKNOWN, 0, &FuncInstr::execute_unknown, 1} } ,

     // **************** R INSTRUCTIONS ****************
    // Constant shifts
    //key      name   format operation  memsize           pointer
    {0x0, { "sll" , FORMAT_R, OUT_R_SHAMT, 0, &FuncInstr::execute_sll, 1} },
    //       0x1 movci
    {0x2, { "srl", FORMAT_R, OUT_R_SHAMT, 0, &FuncInstr::execute_srl, 1} },
    {0x3, { "sra", FORMAT_R, OUT_R_SHAMT, 0, &FuncInstr::execute_sra, 1} },

    // Variable shifts
    //key      name  format operation  memsize           pointer
    {0x4, { "sllv", FORMAT_R, OUT_R_SHIFT, 0, &FuncInstr::execute_sllv, 1} },
    //        0x5 reserved
    {0x6, { "srlv", FORMAT_R, OUT_R_SHIFT, 0, &FuncInstr::execute_srlv, 1} },
    {0x7, { "srav", FORMAT_R, OUT_R_SHIFT, 0, &FuncInstr::execute_srav, 1} },

    // Indirect branches
    //key      name     format operation  memsize           pointer
    {0x8, { "jr"  , FORMAT_R, OUT_R_JUMP,      0, &FuncInstr::execute_jr,   1} },
    {0x9, { "jalr", FORMAT_R, OUT_R_JUMP_LINK, 0, &FuncInstr::execute_jalr, 1} },

    // Conditional moves (MIPS IV)
    //key      name     format operation  memsize           pointer
    {0xA,  { "movz", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_movz, 4} },
    {0xB,  { "movn", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_movn, 4} },

    // System calls
    //key      name     format operation  memsize           pointer
    {0xC, { "syscall", FORMAT_R, OUT_R_SPECIAL, 0, &FuncInstr::execute_syscall, 1} },
    {0xD, { "break",   FORMAT_R, OUT_R_SPECIAL, 0, &FuncInstr::execute_break,   1} },
    //          0xE reserved
    //          0xF SYNC    

    // HI/LO manipulations
    //key      name     format operation  memsize           pointer
    {0x10, { "mfhi", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mfhi, 1} },
    {0x11, { "mthi", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mthi, 1} },
    {0x12, { "mflo", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mflo, 1} },
    {0x13, { "mtlo", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mtlo, 1} },

    // 0x14 - 0x17 double width shifts

    // Multiplication/Division
    //key      name     format operation  memsize           pointer
    {0x18, { "mult",  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_mult,  1} },
    {0x19, { "multu", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_multu, 1} },
    {0x1A, { "div",   FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_div,   1} },
    {0x1B, { "divu",  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_divu,  1} },

    // 0x1C - 0x1F double width multiplication/division

    // Addition/Subtraction
    //key      name     format operation  memsize           pointer
    {0x20, { "add",  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_add,  1} },
    {0x21, { "addu", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_addu, 1} },
    {0x22, { "sub",  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_sub,  1} },
    {0x23, { "subu", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_subu, 1} },

    // Logical operations
    //key      name     format operation  memsize           pointer
    {0x24, { "and",  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_and,  1} },
    {0x25, { "or",   FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_or,   1} },
    {0x26, { "xor",  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_xor,  1} },
    {0x27, { "nor",  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_nor,  1} },
    //        0x28 reserved
    //        0x29 reserved
    {0x2A, { "slt",  FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_slt,  1} },
    {0x2B, { "sltu", FORMAT_R, OUT_R_ARITHM, 0, &FuncInstr::execute_sltu, 1} },

    // 0x2C - 0x2F double width addition/substraction
    
    // Conditional traps (MIPS II)
    //key      name     format operation  memsize           pointer
    {0x30, { "tge",  FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tge,  2} },
    {0x31, { "tgeu", FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tgeu, 2} },
    {0x32, { "tlt",  FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tlt,  2} },
    {0x33, { "tltu", FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tltu, 2} },
    {0x34, { "teq",  FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_teq,  2} },
    //        0x35 reserved
    {0x36, { "tne",  FORMAT_R, OUT_R_TRAP, 0, &FuncInstr::execute_tne,  2} }
    //        0x37 reserved
    // 0x38 - 0x3F double width shifts
};

const std::unordered_map <uint8, FuncInstr::ISAEntry> FuncInstr::isaMapRI =
{
//for RI-instructions
};

//unordered map for I-instructions and J-instructions
const std::unordered_map <uint8, FuncInstr::ISAEntry> FuncInstr::isaMapIJ =
{
    // ********************** REGIMM INSTRUCTIONS *************************

    // ********************* I and J INSTRUCTIONS *************************
    {0xFF, {"###", FORMAT_UNKNOWN, OUT_UNKNOWN, 0, &FuncInstr::execute_unknown, 1} } ,
    // Branches
    //key     name   format    operation  memsize       pointer     
    {0x2, { "j",   FORMAT_J, OUT_J_JUMP,      0, &FuncInstr::execute_j,    1 } },
    {0x3, { "jal", FORMAT_J, OUT_J_JUMP_LINK, 0, &FuncInstr::execute_jal,  1 } },
       
    {0x4, { "beq",  FORMAT_I, OUT_I_BRANCH,    0, &FuncInstr::execute_beq,  1} },
    {0x5, { "bne",  FORMAT_I, OUT_I_BRANCH,    0, &FuncInstr::execute_bne,  1} },
    {0x6, { "blez", FORMAT_I, OUT_I_BRANCH_0,  0, &FuncInstr::execute_blez, 1} },
    {0x7, { "bgtz", FORMAT_I, OUT_I_BRANCH_0,  0, &FuncInstr::execute_bgtz, 1} },

    // Addition/Subtraction
    //key     name   format    operation  memsize       pointer
    {0x8, { "addi",  FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_addi,  1} },
    {0x9, { "addiu", FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_addiu, 1} },

    // Logical operations
    //key     name   format    operation  memsize       pointer
    {0xA, { "slti",  FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_slti,  1} },
    {0xB, { "sltiu", FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_sltiu, 1} },
    {0xC, { "andi",  FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_andi,  1} },
    {0xD, { "ori",   FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_ori,   1} },
    {0xE, { "xori",  FORMAT_I, OUT_I_ARITHM, 0, &FuncInstr::execute_xori,  1} },
    {0xF, { "lui",   FORMAT_I, OUT_I_CONST,  0, &FuncInstr::execute_lui,   1} },

    // 0x10 - 0x13 coprocessor operations 

    // Likely branches (MIPS II)
    //key     name   format    operation  memsize       pointer
    {0x14, { "beql",  FORMAT_I, OUT_I_BRANCH,   0, &FuncInstr::execute_beq,  2} },
    {0x15, { "bnel",  FORMAT_I, OUT_I_BRANCH,   0, &FuncInstr::execute_bne,  2} },
    {0x16, { "blezl", FORMAT_I, OUT_I_BRANCH_0, 0, &FuncInstr::execute_blez, 2} },
    {0x17, { "bgtzl", FORMAT_I, OUT_I_BRANCH_0, 0, &FuncInstr::execute_bgtz, 2} },

    // 0x18 - 0x19 double width addition
    // 0x1A - 0x1B load double word left/right

    // Loads
    //key     name   format    operation  memsize       pointer
    {0x20, { "lb",  FORMAT_I, OUT_I_LOAD,  1, &FuncInstr::calculate_load_addr, 1} },
    {0x21, { "lh",  FORMAT_I, OUT_I_LOAD,  2, &FuncInstr::calculate_load_addr, 1} },
    {0x22, { "lwl", FORMAT_I, OUT_I_LOADL, 4, &FuncInstr::calculate_load_addr, 1} },
    {0x23, { "lw",  FORMAT_I, OUT_I_LOAD,  4, &FuncInstr::calculate_load_addr, 1} },
    {0x24, { "lbu", FORMAT_I, OUT_I_LOADU, 1, &FuncInstr::calculate_load_addr, 1} },
    {0x25, { "lhu", FORMAT_I, OUT_I_LOADU, 2, &FuncInstr::calculate_load_addr, 1} },
    {0x26, { "lwr", FORMAT_I, OUT_I_LOADR, 4, &FuncInstr::calculate_load_addr, 1} },
    {0x27, { "lwu", FORMAT_I, OUT_I_LOADU, 4, &FuncInstr::calculate_load_addr, 1} },

    // Store
    //key     name   format    operation  memsize       pointer
    {0x28, { "sb",  FORMAT_I, OUT_I_STORE,  1, &FuncInstr::calculate_store_addr, 1} },
    {0x29, { "sh",  FORMAT_I, OUT_I_STORE,  2, &FuncInstr::calculate_store_addr, 1} },
    {0x2A, { "swl", FORMAT_I, OUT_I_STOREL, 4, &FuncInstr::calculate_store_addr, 1} },
    {0x2B, { "sw",  FORMAT_I, OUT_I_STORE,  4, &FuncInstr::calculate_store_addr, 1} },
    //       0x2C   store double word left
    //       0x2D   store double word right
    {0x2E, { "swr", FORMAT_I, OUT_I_STORER, 4, &FuncInstr::calculate_store_addr, 1 } }
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
    bool valid = false;
    auto it = isaMapRI.cbegin();
    
    switch ( instr.asR.opcode) 
    {
        case 0x0: // R instruction
            it = isaMapR.find( instr.asR.funct);
            valid = ( it != isaMapR.end());
            break;
            
        case 0x1: // RegIMM instruction
            it = isaMapR.find( instr.asR.opcode);
            valid = ( it != isaMapRI.end());
            break;
            
        default: // I and J instructions
            it = isaMapR.find( instr.asR.opcode);
            valid = ( it != isaMapIJ.end());
            break;
    }
    
    if ( valid) 
    {
        const auto& entry = it->second; 
        
        format    = entry.format;
        operation = entry.operation;
        mem_size  = entry.mem_size;
        name      = entry.name;
        function  = entry.function;       
    }
     
    else 
    {
        initUnknown();
    }
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

