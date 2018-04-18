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
const std::unordered_map <uint8, MIPSInstr::ISAEntry> MIPSInstr::isaMapR =
{
    // **************** R INSTRUCTIONS ****************
    // Constant shifts
    //key      name  operation  memsize           pointer
    {0x0, { "sll" , OUT_R_SHAMT, 0, &MIPSInstr::execute_sll, 1} },
    //       0x1 movci
    {0x2, { "srl", OUT_R_SHAMT, 0, &MIPSInstr::execute_srl, 1} },
    {0x3, { "sra", OUT_R_SHAMT, 0, &MIPSInstr::execute_sra, 1} },

    // Variable shifts
    //key      name  operation  memsize           pointer
    {0x4, { "sllv", OUT_R_SHIFT, 0, &MIPSInstr::execute_sllv, 1} },
    //        0x5 reserved
    {0x6, { "srlv", OUT_R_SHIFT, 0, &MIPSInstr::execute_srlv, 1} },
    {0x7, { "srav", OUT_R_SHIFT, 0, &MIPSInstr::execute_srav, 1} },

    // Indirect branches
    //key      name   operation  memsize           pointer
    {0x8, { "jr"  , OUT_R_JUMP,      0, &MIPSInstr::execute_jr,   1} },
    {0x9, { "jalr", OUT_R_JUMP_LINK, 0, &MIPSInstr::execute_jalr, 1} },

    // Conditional moves (MIPS IV)
    //key      name    operation  memsize           pointer
    {0xA,  { "movz", OUT_R_CONDM, 0, &MIPSInstr::execute_movz, 4} },
    {0xB,  { "movn", OUT_R_CONDM, 0, &MIPSInstr::execute_movn, 4} },

    // System calls
    //key      name     operation  memsize           pointer
    {0xC, { "syscall", OUT_R_SPECIAL, 0, &MIPSInstr::execute_syscall, 1} },
    {0xD, { "break",   OUT_R_SPECIAL, 0, &MIPSInstr::execute_break,   1} },
    //          0xE reserved
    //          0xF SYNC

    // HI/LO manipulations
    //key      name   operation  memsize           pointer
    {0x10, { "mfhi", OUT_R_MFHI, 0, &MIPSInstr::execute_move, 1} },
    {0x11, { "mthi", OUT_R_MTHI, 0, &MIPSInstr::execute_move, 1} },
    {0x12, { "mflo", OUT_R_MFLO, 0, &MIPSInstr::execute_move, 1} },
    {0x13, { "mtlo", OUT_R_MTLO, 0, &MIPSInstr::execute_move, 1} },

    // 0x14 - 0x17 double width shifts

    // Multiplication/Division
    //key      name    operation  memsize           pointer
    {0x18, { "mult",  OUT_R_DIVMULT, 0, &MIPSInstr::execute_mult,  1} },
    {0x19, { "multu", OUT_R_DIVMULT, 0, &MIPSInstr::execute_multu, 1} },
    {0x1A, { "div",   OUT_R_DIVMULT, 0, &MIPSInstr::execute_div,   1} },
    {0x1B, { "divu",  OUT_R_DIVMULT, 0, &MIPSInstr::execute_divu,  1} },

    // 0x1C - 0x1F double width multiplication/division

    // Addition/Subtraction
    //key      name   operation  memsize           pointer
    {0x20, { "add",  OUT_R_ARITHM, 0, &MIPSInstr::execute_add,  1} },
    {0x21, { "addu", OUT_R_ARITHM, 0, &MIPSInstr::execute_addu, 1} },
    {0x22, { "sub",  OUT_R_ARITHM, 0, &MIPSInstr::execute_sub,  1} },
    {0x23, { "subu", OUT_R_ARITHM, 0, &MIPSInstr::execute_subu, 1} },

    // Logical operations
    //key      name   operation  memsize           pointer
    {0x24, { "and", OUT_R_ARITHM, 0, &MIPSInstr::execute_and,  1} },
    {0x25, { "or",  OUT_R_ARITHM, 0, &MIPSInstr::execute_or,   1} },
    {0x26, { "xor", OUT_R_ARITHM, 0, &MIPSInstr::execute_xor,  1} },
    {0x27, { "nor", OUT_R_ARITHM, 0, &MIPSInstr::execute_nor,  1} },
    //        0x28 reserved
    //        0x29 reserved
    {0x2A, { "slt",  OUT_R_ARITHM, 0, &MIPSInstr::execute_set<&MIPSInstr::lt>,  1} },
    {0x2B, { "sltu", OUT_R_ARITHM, 0, &MIPSInstr::execute_set<&MIPSInstr::ltu>, 1} },

    // 0x2C - 0x2F double width addition/substraction

    // Conditional traps (MIPS II)
    //key      name operation  memsize           pointer
    {0x30, { "tge",  OUT_R_TRAP, 0, &MIPSInstr::execute_trap<&MIPSInstr::ge>,  2} },
    {0x31, { "tgeu", OUT_R_TRAP, 0, &MIPSInstr::execute_trap<&MIPSInstr::geu>, 2} },
    {0x32, { "tlt",  OUT_R_TRAP, 0, &MIPSInstr::execute_trap<&MIPSInstr::lt>,  2} },
    {0x33, { "tltu", OUT_R_TRAP, 0, &MIPSInstr::execute_trap<&MIPSInstr::ltu>, 2} },
    {0x34, { "teq",  OUT_R_TRAP, 0, &MIPSInstr::execute_trap<&MIPSInstr::eq>,  2} },
    //        0x35 reserved
    {0x36, { "tne", OUT_R_TRAP, 0, &MIPSInstr::execute_trap<&MIPSInstr::ne>,  2} }
    //        0x37 reserved
    // 0x38 - 0x3F double width shifts
};

//unordered map for RI-instructions
const std::unordered_map <uint8, MIPSInstr::ISAEntry> MIPSInstr::isaMapRI =
{
// ********************** REGIMM INSTRUCTIONS *************************
    // Branches
    //key     name    operation     memsize       pointer
    {0x0,  { "bltz",  OUT_RI_BRANCH_0,  0, &MIPSInstr::execute_branch<&MIPSInstr::ltz>, 1} },
    {0x1,  { "bgez",  OUT_RI_BRANCH_0,  0, &MIPSInstr::execute_branch<&MIPSInstr::gez>, 1} },
    {0x2,  { "bltzl", OUT_RI_BRANCH_0,  0, &MIPSInstr::execute_branch<&MIPSInstr::ltz>, 2} },
    {0x3,  { "bgezl", OUT_RI_BRANCH_0,  0, &MIPSInstr::execute_branch<&MIPSInstr::gez>, 2} },

    {0x8,  { "tgei",  OUT_RI_TRAP,      0, &MIPSInstr::execute_trap<&MIPSInstr::gei>,  2} },
    {0x9,  { "tgeiu", OUT_RI_TRAP,      0, &MIPSInstr::execute_trap<&MIPSInstr::geiu>, 2} },
    {0xA,  { "tlti",  OUT_RI_TRAP,      0, &MIPSInstr::execute_trap<&MIPSInstr::lti>,  2} },
    {0xB,  { "tltiu", OUT_RI_TRAP,      0, &MIPSInstr::execute_trap<&MIPSInstr::ltiu>, 2} },
    {0xC,  { "teqi",  OUT_RI_TRAP,      0, &MIPSInstr::execute_trap<&MIPSInstr::eqi>,  2} },
    {0xE,  { "tnei",  OUT_RI_TRAP,      0, &MIPSInstr::execute_trap<&MIPSInstr::nei>,  2} },

    {0x10, { "bltzal",  OUT_RI_BRANCH_LINK, 0, &MIPSInstr::execute_branch_and_link<&MIPSInstr::ltz>, 1} },
    {0x11, { "bgezal",  OUT_RI_BRANCH_LINK, 0, &MIPSInstr::execute_branch_and_link<&MIPSInstr::gez>, 1} },
    {0x12, { "bltzall", OUT_RI_BRANCH_LINK, 0, &MIPSInstr::execute_branch_and_link<&MIPSInstr::ltz>, 2} },
    {0x13, { "bgezall", OUT_RI_BRANCH_LINK, 0, &MIPSInstr::execute_branch_and_link<&MIPSInstr::gez>, 2} }
};

//unordered map for I-instructions and J-instructions
const std::unordered_map <uint8, MIPSInstr::ISAEntry> MIPSInstr::isaMapIJ =
{
    // ********************* I and J INSTRUCTIONS *************************
    // Branches
    //key     name operation  memsize       pointer
    {0x2, { "j",   OUT_J_JUMP,      0, &MIPSInstr::execute_j,    1 } },
    {0x3, { "jal", OUT_J_JUMP_LINK, 0, &MIPSInstr::execute_jal,  1 } },

    {0x4, { "beq",  OUT_I_BRANCH,    0, &MIPSInstr::execute_branch<&MIPSInstr::eq>,  1} },
    {0x5, { "bne",  OUT_I_BRANCH,    0, &MIPSInstr::execute_branch<&MIPSInstr::ne>,  1} },
    {0x6, { "blez", OUT_I_BRANCH_0,  0, &MIPSInstr::execute_branch<&MIPSInstr::lez>, 1} },
    {0x7, { "bgtz", OUT_I_BRANCH_0,  0, &MIPSInstr::execute_branch<&MIPSInstr::gtz>, 1} },

    // Addition/Subtraction
    //key     name  operation  memsize       pointer
    {0x8, { "addi",  OUT_I_ARITHM, 0, &MIPSInstr::execute_addi,  1} },
    {0x9, { "addiu", OUT_I_ARITHM, 0, &MIPSInstr::execute_addiu, 1} },

    // Logical operations
    //key     name   operation  memsize       pointer
    {0xA, { "slti",  OUT_I_ARITHM, 0, &MIPSInstr::execute_set<&MIPSInstr::lti>,  1} },
    {0xB, { "sltiu", OUT_I_ARITHM, 0, &MIPSInstr::execute_set<&MIPSInstr::ltiu>, 1} },
    {0xC, { "andi",  OUT_I_ARITHM, 0, &MIPSInstr::execute_andi,  1} },
    {0xD, { "ori",  OUT_I_ARITHM, 0, &MIPSInstr::execute_ori,   1} },
    {0xE, { "xori", OUT_I_ARITHM, 0, &MIPSInstr::execute_xori,  1} },
    {0xF, { "lui",  OUT_I_CONST,  0, &MIPSInstr::execute_lui,   1} },

    // 0x10 - 0x13 coprocessor operations

    // Likely branches (MIPS II)
    //key     name   operation  memsize       pointer
    {0x14, { "beql",  OUT_I_BRANCH,   0, &MIPSInstr::execute_branch<&MIPSInstr::eq>,  2} },
    {0x15, { "bnel",  OUT_I_BRANCH,   0, &MIPSInstr::execute_branch<&MIPSInstr::ne>,  2} },
    {0x16, { "blezl", OUT_I_BRANCH_0, 0, &MIPSInstr::execute_branch<&MIPSInstr::lez>, 2} },
    {0x17, { "bgtzl", OUT_I_BRANCH_0, 0, &MIPSInstr::execute_branch<&MIPSInstr::gtz>, 2} },

    // 0x18 - 0x19 double width addition
    // 0x1A - 0x1B load double word left/right

    // Loads
    //key     name  operation         memsize       pointer
    {0x20, { "lb",  OUT_I_LOAD,         1, &MIPSInstr::calculate_load_addr,         1} },
    {0x21, { "lh",  OUT_I_LOAD,         2, &MIPSInstr::calculate_load_addr_aligned, 1} },
    {0x22, { "lwl", OUT_I_PARTIAL_LOAD, 4, &MIPSInstr::calculate_load_addr_left,    1} },
    {0x23, { "lw",  OUT_I_LOAD,         4, &MIPSInstr::calculate_load_addr_aligned, 1} },
    {0x24, { "lbu", OUT_I_LOADU,        1, &MIPSInstr::calculate_load_addr,         1} },
    {0x25, { "lhu", OUT_I_LOADU,        2, &MIPSInstr::calculate_load_addr_aligned, 1} },
    {0x26, { "lwr", OUT_I_PARTIAL_LOAD, 4, &MIPSInstr::calculate_load_addr_right,   1} },
    {0x27, { "lwu", OUT_I_LOADU,        4, &MIPSInstr::calculate_load_addr_aligned, 1} },

    // Stores
    //key     name   operation  memsize       pointer
    {0x28, { "sb",  OUT_I_STORE,  1, &MIPSInstr::calculate_store_addr, 1} },
    {0x29, { "sh",  OUT_I_STORE,  2, &MIPSInstr::calculate_store_addr, 1} },
    {0x2A, { "swl", OUT_I_STOREL, 4, &MIPSInstr::calculate_store_addr, 1} },
    {0x2B, { "sw",  OUT_I_STORE,  4, &MIPSInstr::calculate_store_addr, 1} },
    //       0x2C   store double word left
    //       0x2D   store double word right
    {0x2E, { "swr", OUT_I_STORER, 4, &MIPSInstr::calculate_store_addr, 1 } },
    //       0x2F   cache

    // Advanced loads and stores
    {0x30, { "ll",  OUT_I_LOAD,   4, &MIPSInstr::calculate_load_addr_aligned, 2} },
    {0x38, { "sc",  OUT_I_STORE,  4, &MIPSInstr::calculate_store_addr, 2} },
};

const std::unordered_map <uint8, MIPSInstr::ISAEntry> MIPSInstr::isaMapMIPS32 =
{
    // ********************* MIPS32 INSTRUCTIONS *************************
    //SPECIAL 2
    //key     name    operation  memsize      pointer       mips version
    {0x00, { "madd",  OUT_R_ACCUM,   0, &MIPSInstr::execute_mult,    32} },
    {0x01, { "maddu", OUT_R_ACCUM,   0, &MIPSInstr::execute_multu,   32} },
    {0x02, { "mul",   OUT_R_ARITHM,  0, &MIPSInstr::execute_mult,    32} },
    {0x04, { "msub",  OUT_R_SUBTR,   0, &MIPSInstr::execute_mult,    32} },
    {0x05, { "msubu", OUT_R_SUBTR,   0, &MIPSInstr::execute_multu,   32} },
    {0x20, { "clz",   OUT_SP2_COUNT, 0, &MIPSInstr::execute_clz,     32} },
    {0x21, { "clo",   OUT_SP2_COUNT, 0, &MIPSInstr::execute_clo,     32} },
};

MIPSInstr::MIPSInstr( uint32 bytes, Addr PC) :
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

void MIPSInstr::init( const MIPSInstr::ISAEntry& entry)
{
    operation = entry.operation;
    mem_size  = entry.mem_size;
    function  = entry.function;

    std::ostringstream oss;
    if ( PC != 0)
        oss << std::hex << "0x" << PC << ": ";
    oss << entry.name;

    switch ( operation)
    {
        case OUT_R_MFHI:
            src1 = MIPSRegister::mips_hi;
            dst  = MIPSRegister(instr.asR.rd);
            oss <<  " $" << dst;
            break;
        case OUT_R_MFLO:
            src1 = MIPSRegister::mips_lo;
            dst  = MIPSRegister(instr.asR.rd);
            oss <<  " $" << dst;
            break;
        case OUT_R_MTHI:
            src1 = MIPSRegister(instr.asR.rs);
            dst  = MIPSRegister::mips_hi;
            oss <<  " $" << src1;
            break;
        case OUT_R_MTLO:
            src1 = MIPSRegister(instr.asR.rs);
            dst  = MIPSRegister::mips_lo;
            oss <<  " $" << src1;
            break;
        case OUT_R_SUBTR:
        case OUT_R_ACCUM:
        case OUT_R_DIVMULT:
            src2 = MIPSRegister(instr.asR.rt);
            src1 = MIPSRegister(instr.asR.rs);
            dst  = MIPSRegister::mips_hi_lo;
            oss <<  " $" << src1
                << ", $" << src2;
            break;
        case OUT_R_ARITHM:
        case OUT_R_CONDM:
            src2 = MIPSRegister(instr.asR.rt);
            src1 = MIPSRegister(instr.asR.rs);
            dst  = MIPSRegister(instr.asR.rd);

            oss <<  " $" << dst
                << ", $" << src1
                << ", $" << src2;
            break;
        case OUT_R_SHIFT:
            src2 = MIPSRegister(instr.asR.rs);
            src1 = MIPSRegister(instr.asR.rt);
            dst  = MIPSRegister(instr.asR.rd);

            oss <<  " $" << dst
                << ", $" << src1
                << ", $" << src2;
            break;
        case OUT_R_SHAMT:
            src1  = MIPSRegister(instr.asR.rt);
            dst   = MIPSRegister(instr.asR.rd);
            shamt = instr.asR.shamt;

            oss <<  " $" << dst
                << ", $" << src1
                <<  ", " << std::dec << shamt;
            break;
        case OUT_R_JUMP_LINK:
            src1  = MIPSRegister(instr.asR.rs);
            dst   = MIPSRegister(instr.asR.rd);
            oss <<  " $" << dst
                << ", $" << src1;
            break;
        case OUT_R_JUMP:
            dst = MIPSRegister::zero;
            src1  = MIPSRegister(instr.asR.rs);
            oss << " $" << src1;
            break;
        case OUT_R_TRAP:
            dst = MIPSRegister::zero;
            src1 = MIPSRegister(instr.asR.rs);
            src2 = MIPSRegister(instr.asR.rt);

            oss <<  " $" << src1
                << ", $" << src2;
            break;
        case OUT_RI_TRAP:
            v_imm = instr.asI.imm;
            src1 = MIPSRegister(instr.asI.rs);

            oss << " $" << src1 << ", "
                << std::hex << "0x"
                << static_cast<int16>(v_imm) << std::dec;
            break;
        case OUT_R_SPECIAL:
            break;
        case OUT_I_ARITHM:
            v_imm = instr.asI.imm;
            src1 = MIPSRegister(instr.asI.rs);
            dst  = MIPSRegister(instr.asI.rt);

            oss << " $" << dst << ", $"
                << src1 << ", "
                << std::hex << "0x" << v_imm << std::dec;
            break;
        case OUT_I_BRANCH:
            v_imm = instr.asI.imm;
            src1 = MIPSRegister(instr.asI.rs);
            src2 = MIPSRegister(instr.asI.rt);

            oss << " $" << src1 << ", $"
                << src2 << ", "
                << std::dec << static_cast<int16>(v_imm);
            break;
        case OUT_RI_BRANCH_0:
            v_imm = instr.asI.imm;
            src1 = MIPSRegister(instr.asI.rs);
            oss << " $" << src1 << ", "
                << std::dec << static_cast<int16>(v_imm);
            break;
        case OUT_I_BRANCH_0:
            v_imm = instr.asI.imm;
            src1 = MIPSRegister(instr.asI.rs);

            oss << " $" << src1 << ", "
                << std::dec << static_cast<int16>(v_imm);
            break;
        case OUT_I_CONST:
            v_imm = instr.asI.imm;
            dst  = MIPSRegister(instr.asI.rt);

            oss << " $" << dst << std::hex
                << ", 0x" << v_imm << std::dec;
            break;

        case OUT_I_LOAD:
        case OUT_I_LOADU:
        case OUT_I_PARTIAL_LOAD:
            v_imm = instr.asI.imm;
            src1 = MIPSRegister(instr.asI.rs);
            dst  = MIPSRegister(instr.asI.rt);

            oss << " $" << dst << ", 0x"
                << std::hex << v_imm
                << "($" << src1 << ")" << std::dec;
            break;

        case OUT_I_STORE:
        case OUT_I_STOREL:
        case OUT_I_STORER:
            v_imm = instr.asI.imm;
            src2 = MIPSRegister(instr.asI.rt);
            src1 = MIPSRegister(instr.asI.rs);
            dst  = MIPSRegister::zero;

            oss << " $" << src2 << ", 0x"
                << std::hex << v_imm
                << "($" << src1 << ")" << std::dec;
            break;
        case OUT_RI_BRANCH_LINK:
            v_imm = instr.asI.imm;
            src1 = MIPSRegister(instr.asI.rs);
            dst = MIPSRegister::return_address;
            oss << " $" << src1 << ", "
                << std::dec << static_cast<int16>(v_imm);
            break;
        case OUT_J_JUMP_LINK:
            v_imm = instr.asJ.imm;
            dst = MIPSRegister::return_address;
            oss << " 0x"
                << std::hex << static_cast<uint16>(v_imm) << std::dec;
            break;
        case OUT_J_JUMP:
            v_imm = instr.asJ.imm;
            dst = MIPSRegister::zero;
            oss << " 0x"
                << std::hex << static_cast<uint16>(v_imm) << std::dec;
            break;
        case OUT_SP2_COUNT:
            src1 = MIPSRegister(instr.asR.rs);
            dst  = MIPSRegister(instr.asR.rd);

            oss <<  " $" << dst
                << ", $" << src1;
            break;
        default:
            assert( false);
    }
    if ( instr.raw == 0x0ul)
        disasm = "nop ";
    else
        disasm = oss.str();
}

void MIPSInstr::execute_unknown()
{
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}

void MIPSInstr::execute()
{
    (this->*function)();
    complete = true;

    if ( !dst.is_zero() && !is_load() && get_mask() != 0)
    {
        std::ostringstream oss;
        oss << "\t [ $" << std::hex;
        if ( dst.is_mips_hi_lo())
            oss <<  MIPSRegister::mips_hi << " = 0x" << static_cast<uint32>( v_dst >> 32u) << ", $"
                <<  MIPSRegister::mips_lo;
        else
            oss <<  dst;

        oss << " = 0x" << static_cast<uint32>( v_dst & mask) << " ]";
        disasm += oss.str();
    }
}

void MIPSInstr::set_v_dst( uint32 value)
{
    if ( operation == OUT_I_LOAD || is_partial_load())
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

    if ( !dst.is_zero())
    {
        std::ostringstream oss;
        oss << "\t [ $" << dst << " = 0x" << std::hex << v_dst
        if (get_mask() != all_ones<uint32>())
            oss << ", mask = 0x" << std::hex << mask;
        oss << "]";
        disasm += oss.str();
    }
}

void MIPSInstr::check_trap()
{
    if ( trap != TrapType::NO_TRAP)
    {
        std::ostringstream oss;
        oss << "\t trap";
        disasm += oss.str();
    }
}
