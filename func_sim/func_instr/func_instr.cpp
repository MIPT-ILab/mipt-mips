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
    { "add"     ,    0,     0x20, R_FMT, ADD    , true, true, true, false   },
    { "addu"    ,    0,     0x21, R_FMT, ADDU   , true, true, true, false   },
    { "sub"     ,    0,     0x22, R_FMT, SUB    , true, true, true, false   },
    { "subu"    ,    0,     0x23, R_FMT, SUBU   , true, true, true, false   },
    { "addi"    ,  0x8,        0, I_FMT, ADDI                               },
    { "addiu"   ,  0x9,        0, I_FMT, ADDIU                              },
    { "mult"    ,    0,     0x18, R_FMT, MULT                               },
    { "multu"   ,    0,     0x19, R_FMT, MULTU                              },
    { "div"     ,    0,     0x1A, R_FMT, DIV                                },
    { "divu"    ,    0,     0x1B, R_FMT, DIVU                               },
    { "mhfi"    ,    0,     0x10, R_FMT, MHFI                               },
    { "mthi"    ,    0,     0x11, R_FMT, MTHI                               },
    { "mflo"    ,    0,     0x12, R_FMT, MFLO                               },
    { "mtlo"    ,    0,     0x13, R_FMT, MTLO                               },
    { "sll"     ,    0,      0x0, R_FMT, SLL                                },
    { "srl"     ,    0,      0x2, R_FMT, SRL                                },
    { "sra"     ,    0,      0x3, R_FMT, SRA                                },
    { "sllv"    ,    0,      0x4, R_FMT, SLLV                               },
    { "srlv"    ,    0,      0x6, R_FMT, SRLV                               },
    { "srav"    ,    0,      0x7, R_FMT, SRAV                               },
    { "lui"     ,  0xF,        0, I_FMT, LUI                                },
    { "slt"     ,    0,     0x2A, R_FMT, SLT                                },
    { "sltu"    ,    0,     0x2B, R_FMT, SLTU                               },
    { "slti"    ,  0xA,        0, I_FMT, SLTI                               },
    { "sltiu"   ,  0xB,        0, I_FMT, SLTIU                              },
    { "and"     ,    0,     0x24, R_FMT, AND                                },
    { "or"      ,    0,     0x25, R_FMT, OR                                 },
    { "xor"     ,    0,     0x26, R_FMT, XOR                                },
    { "nor"     ,    0,     0x27, R_FMT, NOR                                },
    { "andi"    ,  0xC,        0, I_FMT, ANDI                               },
    { "ori"     ,  0xD,        0, I_FMT, ORI                                },
    { "xori"    ,  0xE,        0, I_FMT, XORI                               },
    { "nor"     ,    0,     0x27, R_FMT, NOR                                },
    { "andi"    ,  0xC,        0, I_FMT, ANDI                               },
    { "ori"     ,  0xD,        0, I_FMT, ORI                                },
    { "xori"    ,  0xE,        0, I_FMT, XORI                               },
    { "beq"     ,  0x4,        0, I_FMT, BEQ                                },
    { "bne"     ,  0x5,        0, I_FMT, BNE                                },
    { "blez"    ,  0x6,        0, I_FMT, BLEZ                               },
    { "bgtz"    ,  0x7,        0, I_FMT, BGTZ                               },
    { "j"       ,  0x2,        0, J_FMT, J                                  },
    { "jal"     ,  0x3,        0, J_FMT, JAL                                },
    { "jr"      ,    0,      0x8, R_FMT, JR                                 },
    { "jalr"    ,    0,      0x9, R_FMT, JALR                               },
    { "lb"      , 0x20,        0, I_FMT, LB                                 },
    { "lh"      , 0x21,        0, I_FMT, LH                                 },
    { "lw"      , 0x23,        0, I_FMT, LW                                 },
    { "lbu"     , 0x24,        0, I_FMT, LBU                                },
    { "lhu"     , 0x25,        0, I_FMT, LHU                                },
    { "sb"      , 0x28,        0, I_FMT, SB                                 },
    { "sh"      , 0x29,        0, I_FMT, SH                                 },
    { "sw"      , 0x2b,        0, I_FMT, SW                                 },
    { "syscall" ,    0,      0xC, R_FMT, SYSCALL                            },
    { "break"   ,    0,      0xD, R_FMT, BREAK                              },
    { "trap"    , 0x1A,        0, J_FMT, TRAP                               }
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
    oss << name << " " 
        << regTable[instr.asR.rd].name << ", " 
        << regTable[instr.asR.rs].name << ", " 
        << regTable[instr.asR.rt].name; 
    disasm_string = oss.str();
}

inline void FuncInstr::processAsI()
{
    std::ostringstream oss;
    oss << name << " " 
        << regTable[instr.asI.rt].name << ", " 
        << regTable[instr.asI.rs].name << ", " 
        << "0x" << hex << instr.asI.imm;        
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
