//Generic C
#include <cassert>
#include <cstdlib>
#include <stdio.h>
//uArchSim modules
#include <func_instr.h>

const RegEntry FuncInstr::regTable[] = {
    { "$zero", 0 },
    { "$at",   1 },
    { "$v0",   2 },
    { "$v1",   3 },
    { "$a0",   4 }, 
    { "$a1",   5 },
    { "$a2",   6 },
    { "$a3",   7 },
    { "$t0",   8 },
    { "$t1",   9 },
    { "$t2",  10 },
    { "$t3",  11 },
    { "$t4",  12 },
    { "$t5",  13 },
    { "$t6",  14 },
    { "$t7",  15 },
    { "$s0",  16 },
    { "$s1",  17 },
    { "$s2",  18 },
    { "$s3",  19 },
    { "$s4",  20 },
    { "$s5",  21 },
    { "$s6",  22 },
    { "$s7",  23 },
    { "$t8",  24 },
    { "$t9",  25 },
    { "$k0",  26 },
    { "$k1",  27 },
    { "$gp",  28 },
    { "$sp",  29 },
    { "$s8",  30 },
    { "$ra",  31 }
};

const ISAEntry FuncInstr::isaTable[] = {
//    name       opcode     func  format type     rs     rt     rd     shamt  brackets
    { "add"     ,    0,     0x20, R_FMT, ADD    , true , true , true , false, false },
    { "addu"    ,    0,     0x21, R_FMT, ADDU   , true , true , true , false, false },
    { "sub"     ,    0,     0x22, R_FMT, SUB    , true , true , true , false, false },
    { "subu"    ,    0,     0x23, R_FMT, SUBU   , true , true , true , false, false },
    { "addi"    ,  0x8,        0, I_FMT, ADDI   , true , true , false, false, false },
    { "addiu"   ,  0x9,        0, I_FMT, ADDIU  , true , true , false, false, false },
    { "mult"    ,    0,     0x18, R_FMT, MULT   , true , true , false, false, false },
    { "multu"   ,    0,     0x19, R_FMT, MULTU  , true , true , false, false, false },
    { "div"     ,    0,     0x1A, R_FMT, DIV    , true , true , false, false, false },
    { "divu"    ,    0,     0x1B, R_FMT, DIVU   , true , true , false, false, false },
    { "mfhi"    ,    0,     0x10, R_FMT, MFHI   , false, false, true , false, false },
    { "mthi"    ,    0,     0x11, R_FMT, MTHI   , true , false, false, false, false },
    { "mflo"    ,    0,     0x12, R_FMT, MFLO   , false, false, true , false, false },
    { "mtlo"    ,    0,     0x13, R_FMT, MTLO   , true , false, false, false, false },
    { "sll"     ,    0,      0x0, R_FMT, SLL    , false, true , true , true , false },
    { "srl"     ,    0,      0x2, R_FMT, SRL    , false, true , true , true , false },
    { "sra"     ,    0,      0x3, R_FMT, SRA    , false, true , true , true , false },
    { "sllv"    ,    0,      0x4, R_FMT, SLLV   , true , true , true , false, false },
    { "srlv"    ,    0,      0x6, R_FMT, SRLV   , true , true , true , false, false },
    { "srav"    ,    0,      0x7, R_FMT, SRAV   , true , true , true , false, false },
    { "lui"     ,  0xF,        0, I_FMT, LUI    , false, true , false, false, false },
    { "slt"     ,    0,     0x2A, R_FMT, SLT    , true , true , true , false, false },
    { "sltu"    ,    0,     0x2B, R_FMT, SLTU   , true , true , true , false, false },
    { "slti"    ,  0xA,        0, I_FMT, SLTI   , true , true , false, false, false },
    { "sltiu"   ,  0xB,        0, I_FMT, SLTIU  , true , true , false, false, false },
    { "and"     ,    0,     0x24, R_FMT, AND    , true , true , true , false, false },
    { "or"      ,    0,     0x25, R_FMT, OR     , true , true , true , false, false },
    { "xor"     ,    0,     0x26, R_FMT, XOR    , true , true , true , false, false },
    { "nor"     ,    0,     0x27, R_FMT, NOR    , true , true , true , false, false },
    { "andi"    ,  0xC,        0, I_FMT, ANDI   , true , true , false, false, false },
    { "ori"     ,  0xD,        0, I_FMT, ORI    , true , true , false, false, false },
    { "xori"    ,  0xE,        0, I_FMT, XORI   , true , true , false, false, false },
    { "beq"     ,  0x4,        0, I_FMT, BEQ    , true , true , false, false, false },
    { "bne"     ,  0x5,        0, I_FMT, BNE    , true , true , false, false, false },
    { "blez"    ,  0x6,        0, I_FMT, BLEZ   , true , false, false, false, false },
    { "bgtz"    ,  0x7,        0, I_FMT, BGTZ   , true , false, false, false, false },
    { "j"       ,  0x2,        0, J_FMT, J      , false, false, false, false, false },
    { "jal"     ,  0x3,        0, J_FMT, JAL    , false, false, false, false, false },
    { "jr"      ,    0,      0x8, R_FMT, JR     , true , false, false, false, false },
    { "jalr"    ,    0,      0x9, R_FMT, JALR   , true , false, false, false, false },
    { "lb"      , 0x20,        0, I_FMT, LB     , true , true , false, false, true  },
    { "lh"      , 0x21,        0, I_FMT, LH     , true , true , false, false, true  },
    { "lw"      , 0x23,        0, I_FMT, LW     , true , true , false, false, true  },
    { "lbu"     , 0x24,        0, I_FMT, LBU    , true , true , false, false, true  },
    { "lhu"     , 0x25,        0, I_FMT, LHU    , true , true , false, false, true  },
    { "sb"      , 0x28,        0, I_FMT, SB     , true , true , false, false, true  },
    { "sh"      , 0x29,        0, I_FMT, SH     , true , true , false, false, true  },
    { "sw"      , 0x2b,        0, I_FMT, SW     , true , true , false, false, true  },
    { "syscall" ,    0,      0xC, R_FMT, SYSCALL, false, false, false, false, false },
    { "break"   ,    0,      0xD, R_FMT, BREAK  , false, false, false, false, false },
    { "trap"    , 0x1A,        0, J_FMT, TRAP   , false, false, false, false, false },
};

FuncInstr::FuncInstr( uint32 bytes)
{
    instr.raw = bytes;
    format = INVALID_FMT;

    if ( instr.asR.opcode == 0)
    {
        format = R_FMT;
        for ( size_t i = 0; i < sizeof( isaTable) / sizeof( ISAEntry); i++)
        {
            if ( ( isaTable[i].func == instr.asR.func) && ( isaTable[i].format == R_FMT))
            {
                type = isaTable[i].type;
                name = isaTable[i].name;
                rs   = isaTable[i].rs;
                rt   = isaTable[i].rt;
                rd   = isaTable[i].rd;
                shamt = isaTable[i].shamt;
                brackets = isaTable[i].brackets; 
		        break;
            }
        }   
    }
    else
    {        
        for ( size_t i = 0; i < sizeof( isaTable) / sizeof( ISAEntry); i++)
        {
            if ( isaTable[i].opcode == instr.asR.opcode)
            {
                format = isaTable[i].format;
                type = isaTable[i].type;
                name = isaTable[i].name;
                rs   = isaTable[i].rs;
                rt   = isaTable[i].rt;
                rd   = isaTable[i].rd;
                shamt = isaTable[i].shamt;
                brackets = isaTable[i].brackets; 
		        break;
            }
        }   
    } 

    switch (format)
    {
    case INVALID_FMT:
        cerr << "ERROR: Invalid instruction." << endl;
        exit( EXIT_FAILURE);
        break; 
    case R_FMT:
        processAsR();
        break;
    case I_FMT:
        processAsI();
        break;
    case J_FMT:
        processAsJ();
        break;
    }
}

inline void FuncInstr::processAsR()
{
    std::ostringstream oss;
    oss << name << " ";
    if (rd)
    {
       oss << regTable[instr.asR.rd].name << ", ";  
    }
    if (rs)
    {
       oss << regTable[instr.asR.rs].name << ", "; 
    }
    if (rt)
    {
       oss << regTable[instr.asR.rt].name;     
    }
    if (shamt)
    {
       oss << ", " << hex << instr.asR.shamt;
    }
    disasm_string = oss.str();
}

inline void FuncInstr::processAsI()
{
    std::ostringstream oss;
    if (brackets)
    {
        oss << name << " ";
        if (rt)
        { 
            oss << regTable[instr.asI.rt].name << ", ";
        }            
        oss << instr.asI.imm << "(";
        if (rs)
        {
            oss << regTable[instr.asI.rs].name << ")";
        }         
    }
    else
    {
        oss << name << " "; 
        if (rt)
        {
            oss << regTable[instr.asI.rt].name << ", ";
        }
        if (rs)
        {
            oss << regTable[instr.asI.rs].name << ", ";
        } 
        oss << "0x" << hex << instr.asI.imm;        
    }
    disasm_string = oss.str();
}

inline void FuncInstr::processAsJ()
{
    std::ostringstream oss;
    oss << name << " 0x" << hex << instr.asJ.addr;
    disasm_string = oss.str();             
}

FuncInstr::~FuncInstr()
{
    
}

std::string FuncInstr::Dump( std::string indent)
{
    std::ostringstream oss;
    oss << indent << disasm_string;    
    return oss.str();
}
