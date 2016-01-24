/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */


#include <func_instr.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name  opcode  func   format    operation
    { "add",    0x0, 0x20,  FORMAT_R, OUT_R_ARITHM},
    { "addu",   0x0, 0x21,  FORMAT_R, OUT_R_ARITHM},
    { "sub",    0x0, 0x22,  FORMAT_R, OUT_R_ARITHM},
    { "subu",   0x0, 0x23,  FORMAT_R, OUT_R_ARITHM},
    { "addi",   0x8, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "addiu",  0x9, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "mult",   0x0, 0x18,  FORMAT_R, OUT_R_ARITHM},    
    { "multu",  0x0, 0x19,  FORMAT_R, OUT_R_ARITHM},
    { "div",    0x0, 0x1A,  FORMAT_R, OUT_R_ARITHM},
    { "divu",   0x0, 0x1B,  FORMAT_R, OUT_R_ARITHM},
    { "mfhi",   0x0, 0x10,  FORMAT_R, OUT_R_ARITHM},
    { "mthi",   0x0, 0x11,  FORMAT_R, OUT_R_ARITHM},
    { "mflo",   0x0, 0x12,  FORMAT_R, OUT_R_ARITHM},
    { "mtlo",   0x0, 0x13,  FORMAT_R, OUT_R_ARITHM},
    { "sll",    0x0, 0x0,   FORMAT_R, OUT_R_SHAMT},
    { "srl",    0x0, 0x2,   FORMAT_R, OUT_R_SHAMT},
    { "sra",    0x0, 0x3,   FORMAT_R, OUT_R_SHAMT},
    { "sllv",   0x0, 0x4,   FORMAT_R, OUT_R_ARITHM},
    { "srlv",   0x0, 0x6,   FORMAT_R, OUT_R_ARITHM},
    { "srav",   0x0, 0x7,   FORMAT_R, OUT_R_ARITHM},
    { "lui",    0xF, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "slt",    0x0, 0x2A,  FORMAT_R, OUT_R_ARITHM},
    { "sltu",   0x0, 0x2B,  FORMAT_R, OUT_R_ARITHM},
    { "slti",   0xA, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "sltiu",  0xB, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "and",    0x0, 0x24,  FORMAT_R, OUT_R_ARITHM},
    { "or",     0x0, 0x25,  FORMAT_R, OUT_R_ARITHM},
    { "xor",    0x0, 0x26,  FORMAT_R, OUT_R_ARITHM},
    { "nor",    0x0, 0x27,  FORMAT_R, OUT_R_ARITHM},
    { "andi",   0xC, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "ori",    0xD, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "xori",   0xE, 0x0,   FORMAT_I, OUT_I_ARITHM},
    { "beq",    0x4, 0x0,   FORMAT_I, OUT_I_BRANCH},
    { "bne",    0x5, 0x0,   FORMAT_I, OUT_I_BRANCH},
    { "blez",   0x6, 0x0,   FORMAT_I, OUT_I_BRANCH},
    { "bgtz",   0x7, 0x0,   FORMAT_I, OUT_I_BRANCH},
    { "jal",    0x3, 0x0,   FORMAT_J, OUT_J_JUMP},
    { "j",      0x2, 0x0,   FORMAT_J, OUT_J_JUMP},
    { "jr",     0x0, 0x8,   FORMAT_R, OUT_R_JUMP},
    { "jalr",   0x0, 0x9,   FORMAT_R, OUT_R_JUMP},
    { "lb",     0x20,0x0,   FORMAT_I, OUT_I_LOAD},
    { "lh",     0x21,0x0,   FORMAT_I, OUT_I_LOAD},
    { "lw",     0x23,0x0,   FORMAT_I, OUT_I_LOAD},
    { "lbu",    0x24,0x0,   FORMAT_I, OUT_I_LOAD},
    { "lhu",    0x25,0x0,   FORMAT_I, OUT_I_LOAD},
    { "sb",     0x28,0x0,   FORMAT_I, OUT_I_STORE},
    { "sh",     0x29,0x0,   FORMAT_I, OUT_I_STORE},
    { "sw",     0x2b,0x0,   FORMAT_I, OUT_I_STORE},
    { "syscall",0x0, 0xC,   FORMAT_R, OUT_R_SPECIAL},
    { "break",  0x0, 0xD,   FORMAT_R, OUT_R_SPECIAL},
    { "trap",   0x1A,0x0,   FORMAT_J, OUT_J_SPECIAL},
};
const uint32 FuncInstr::isaTableSize = sizeof(isaTable) / sizeof(isaTable[0]);

const char *FuncInstr::regTable[] = 
{
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
};

FuncInstr::FuncInstr( uint32 bytes) : instr(bytes)
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
}

std::string FuncInstr::Dump( std::string indent) const
{
    return indent + disasm;
}

void FuncInstr::initFormat()
{
    for ( size_t i = 0; i < isaTableSize; i++) {
        if ( instr.asR.opcode == isaTable[i].opcode)
        {
            format = isaTable[i].format;
            operation = isaTable[i].operation;
            isaNum = i;
            return;
        }
    }
    format = FORMAT_UNKNOWN;
}


void FuncInstr::initR()
{
    // find instr by functor
    for (isaNum = 0; isaNum < isaTableSize; ++isaNum) {
        if (( instr.asR.opcode == isaTable[isaNum].opcode) &&
            ( instr.asR.funct == isaTable[isaNum].funct))
        {
            operation = isaTable[isaNum].operation;
            break;
        }
    }
    if ( isaNum == isaTableSize)     // if didn't found funct
    {
        initUnknown();
        return;
    }
    
    ostringstream oss;
    oss << isaTable[isaNum].name;
    switch ( operation)
    {
        case OUT_R_ARITHM:
            oss << " $" << regTable[instr.asR.rd] << ", $" \
                        << regTable[instr.asR.rs] << ", $" \
                        << regTable[instr.asR.rt];
            break;
        case OUT_R_SHAMT:
            oss << " $" << regTable[instr.asR.rd] << ", $" \
                        << regTable[instr.asR.rt] << ", " \
                        << dec << instr.asR.shamt;
            break;
        case OUT_R_JUMP:
            oss << " $" << regTable[instr.asR.rs];
            break;
        case OUT_R_SPECIAL:
            break;
    }
    disasm = oss.str();
}


void FuncInstr::initI()
{
    std::ostringstream oss;
    oss << isaTable[isaNum].name << " $";
    switch ( operation)
    {
        case OUT_I_ARITHM:
            oss << regTable[instr.asI.rt] << ", $"
                << regTable[instr.asI.rs] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_BRANCH:
            oss << regTable[instr.asI.rs] << ", $"
                << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_LOAD:
            oss << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) 
                << std::dec << "($" << regTable[instr.asI.rs] << ")";
            break;
        case OUT_I_STORE:
            oss << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) 
                << std::dec << "($" << regTable[instr.asI.rs] << ")";
            break;
    }
    disasm = oss.str();
}

void FuncInstr::initJ()
{
    std::ostringstream oss;
    oss << isaTable[isaNum].name;
    switch ( operation)
    {
        case OUT_J_JUMP:
            oss << " " << std::hex << "0x" <<instr.asJ.imm;
            break;
        case OUT_J_SPECIAL:
            break;
    }
    disasm = oss.str();
}

void FuncInstr::initUnknown()
{
    std::ostringstream oss;
    oss << std::hex << std::setfill( '0')
        << "0x" << std::setw( 8) << instr.raw << '\t' << "Unknown" << std::endl;
    disasm = oss.str();
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}

