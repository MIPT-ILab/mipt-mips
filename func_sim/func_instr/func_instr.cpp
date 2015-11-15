/**
 * func_instr.cpp - the module implementing the concept of MIPS
 * disassembler.
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2015 uArchSim iLab project
 */

//Generic C
#include <assert.h> 
#include <string.h>

//Generic C++
#include <sstream>
#include <iostream>
#include <fstream>

//uArchSim modules
#include <func_instr.h>

ofstream mylog( "log.txt");

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name       opcode  func    format               type
    { "add ",     0x0,    0x20,   FuncInstr::FORMAT_R, FuncInstr::ADD},
    { "addu ",    0x0,    0x21,   FuncInstr::FORMAT_R, FuncInstr::ADDU},
    { "sub ",     0x0,    0x22,   FuncInstr::FORMAT_R, FuncInstr::SUB},
    { "subu ",    0x0,    0x23,   FuncInstr::FORMAT_R, FuncInstr::SUBU},
    { "addi ",    0x8,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ADDI},
    { "addiu ",   0x9,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ADDIU},
    { "mult ",    0x0,    0x18,   FuncInstr::FORMAT_R, FuncInstr::MULT},
    { "multu ",   0x0,    0x19,   FuncInstr::FORMAT_R, FuncInstr::MULTU},
    { "div ",     0x0,    0x1A,   FuncInstr::FORMAT_R, FuncInstr::DIV},
    { "divu ",    0x0,    0x1B,   FuncInstr::FORMAT_R, FuncInstr::DIVU},
    { "mfhi ",    0x0,    0x10,   FuncInstr::FORMAT_R, FuncInstr::MFHI},
    { "mthi ",    0x0,    0x11,   FuncInstr::FORMAT_R, FuncInstr::MTHI},
    { "mflo ",    0x0,    0x12,   FuncInstr::FORMAT_R, FuncInstr::MFLO},
    { "mtlo ",    0x0,    0x13,   FuncInstr::FORMAT_R, FuncInstr::MTLO},
    { "sll ",     0x0,    0x0,    FuncInstr::FORMAT_R, FuncInstr::SLL},
    { "srl ",     0x0,    0x2,    FuncInstr::FORMAT_R, FuncInstr::SRL},
    { "sra ",     0x0,    0x3,    FuncInstr::FORMAT_R, FuncInstr::SRA},
    { "sllv ",    0x0,    0x4,    FuncInstr::FORMAT_R, FuncInstr::SLLV},
    { "srlv ",    0x0,    0x6,    FuncInstr::FORMAT_R, FuncInstr::SRLV},
    { "srav ",    0x0,    0x7,    FuncInstr::FORMAT_R, FuncInstr::SRAV},
    { "lui ",     0xF,    0x0,    FuncInstr::FORMAT_I, FuncInstr::LUI},
    { "slt ",     0x0,    0x2A,   FuncInstr::FORMAT_R, FuncInstr::SLT},
    { "sltu ",    0x0,    0x2B,   FuncInstr::FORMAT_R, FuncInstr::SLTU},
    { "slti ",    0xA,    0x0,    FuncInstr::FORMAT_I, FuncInstr::SLTI},
    { "sltiu ",   0xB,    0x0,    FuncInstr::FORMAT_I, FuncInstr::SLTIU},
    { "and ",     0x0,    0x24,   FuncInstr::FORMAT_R, FuncInstr::AND},
    { "or ",      0x0,    0x25,   FuncInstr::FORMAT_R, FuncInstr::OR},
    { "xor ",     0x0,    0x26,   FuncInstr::FORMAT_R, FuncInstr::XOR},
    { "nor ",     0x0,    0x27,   FuncInstr::FORMAT_R, FuncInstr::NOR},
    { "andi ",    0xC,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ANDI},
    { "ori ",     0xD,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ORI},
    { "xori ",    0xE,    0x0,    FuncInstr::FORMAT_I, FuncInstr::XORI},
    { "beq ",     0x4,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BEQ},
    { "bne ",     0x5,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BNE},
    { "blez ",    0x6,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BLEZ},
    { "bgtz ",    0x7,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BGTZ},
    { "j ",       0x2,    0x0,    FuncInstr::FORMAT_J, FuncInstr::J},
    { "jal ",     0x3,    0x0,    FuncInstr::FORMAT_J, FuncInstr::JAL},
    { "jr ",      0x0,    0x8,    FuncInstr::FORMAT_R, FuncInstr::JR},
    { "jalr ",    0x0,    0x9,    FuncInstr::FORMAT_R, FuncInstr::JALR},
    { "lb ",      0x20,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LB},
    { "lh ",      0x21,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LH},
    { "lw ",      0x23,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LW},
    { "lbu ",     0x24,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LBU},
    { "lhu ",     0x25,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LHU},
    { "sb ",      0x28,   0x0,    FuncInstr::FORMAT_I, FuncInstr::SB},
    { "sh ",      0x29,   0x0,    FuncInstr::FORMAT_I, FuncInstr::SH},
    { "sw ",      0x2B,   0x0,    FuncInstr::FORMAT_I, FuncInstr::SW},
    { "syscall ", 0x0,    0xC,    FuncInstr::FORMAT_R, FuncInstr::SYSCALL},
    { "break ",   0x0,    0xD,    FuncInstr::FORMAT_R, FuncInstr::BREAK},
    { "trap ",    0x0,    0x1A,   FuncInstr::FORMAT_J, FuncInstr::TRAP}
};

FuncInstr::FuncInstr( uint32 bytes_arg) 
{
    mylog << "Entering FuncInstr::FuncInstr" << endl;
    this->bytes.raw = bytes_arg;
    this->initFormat(bytes.raw);
    mylog << "Format: " << strFormat( this->format) << endl;
    switch ( this->format)
    {
        case FORMAT_R:
            this->parseR(bytes.raw);
            break;
        case FORMAT_I:
            this->parseI(bytes.raw);
            break;
        case FORMAT_J:
            this->parseJ(bytes.raw);
            break;
        default:
            cerr << "ERROR.Unexpected format type" << endl;
            exit( EXIT_FAILURE);
            break;
    }
    mylog << "Command type: " << this->type << endl;

    ostringstream os;

    os << this->cmd_name;
    switch ( this->format)
    {
        case FORMAT_J:
            os << hex << "0x" << this->bytes.asJ.offset;
            break;
        case FORMAT_I:
            if ( usesTwoOperandsAndOffset( this->type))
            {
                os << asReg( this->bytes.asI.rt) << ", " 
                   << asRegOff( this->bytes.asI.rs, this->bytes.asI.imm) ;
            }
            else if ( usesTwoRegOperandsAndOneImm( this->type))
            {
                os << asReg( this->bytes.asI.rt) << ", "
                   << asReg( this->bytes.asI.rs) << ", "
                   << hex << "0x" << this->bytes.asI.imm;
            }
            else if ( usesOneRegOperandAndOneImm( this->type))
            {
                os << asReg( this->bytes.asR.rt) << ", "
                   << "0x" << hex << this->bytes.asR.rs;
            }
            else
            {
                assert( NULL);
            }
            break;
        case FORMAT_R:
            if ( usesThreeRegOperands( this->type))
            {
                os << asReg( this->bytes.asR.rd) << ", "
                   << asReg( this->bytes.asR.rs) << ", "
                   << asReg( this->bytes.asR.rt);
            }
            else if ( usesTwoRegOperands( this->type))
            {
                os << asReg( this->bytes.asR.rd) << ", "
                   << asReg( this->bytes.asR.rs);
            }
            else if ( usesTwoRegOperandsAndOneImm( this->type))
            {
                os << asReg( this->bytes.asR.rd) << ", "
                   << asReg( this->bytes.asR.rs) << ", "
                   << "0x" << hex << this->bytes.asR.rt;
            }
            else if ( usesOneRegOperand( this->type))
            {
                os << asReg( this->bytes.asR.rt);
            }
            else if ( usesOneRegOperandAndOneImm( this->type))
            {
                os << asReg( this->bytes.asR.rt) << ", "
                   << "0x" << hex << this->bytes.asR.rs;
            }
            else
            {
                assert( NULL);
            }
            break;
        default:
            assert( NULL);
            break;
    }

    full_cmd_name = os.str();
    mylog << "Exiting FuncInstr::FuncInstr" << endl;
}

void FuncInstr::initFormat( uint32 bytes)
{
    uint32 first6 = bytes >> 26;
    if ( first6 == 0)
    {
        this->format=FORMAT_R;
    }
    else if ( first6 == 2 || first6 == 3 || first6 == 0x1A)
    {
        this->format=FORMAT_J;
    }
    else
    {
        this->format=FORMAT_I;
    }
}

void FuncInstr::parseR( uint32 bytes)
{
    mylog << "Entering FuncInstr::parseR (no quit msg)" << endl;
    for ( unsigned i = 0; i < sizeof( isaTable) / sizeof( isaTable[0]); ++i)
    {
        mylog << "searched funct: " << this->bytes.asR.funct
             << ", current funct: " << (int)isaTable[i].func
             << ", current format: " << strFormat( isaTable[i].format)
             << endl;
        if ( this->bytes.asR.funct == isaTable[i].func
                && isaTable[i].format == FORMAT_R )
        {
            this->type = isaTable[i].type;
            this->cmd_name = ( char*)isaTable[i].name;
            return;
        }
    }
    cerr << "ERROR.Invalid R command" << endl;
    exit( EXIT_FAILURE); //this string never must be executed
}

void FuncInstr::parseJ( uint32 bytes)
{
    for ( unsigned i = 0; i < sizeof( isaTable) / sizeof( isaTable[0]); ++i)
    {
        if ( this->bytes.asJ.op == isaTable[i].opcode
            && isaTable[i].format == FORMAT_J )
        {
            this->type = isaTable[i].type;
            this->cmd_name = ( char*)isaTable[i].name;
            return;
        }
    }
    cerr << "ERROR.Invalid J command" << endl;
    exit( EXIT_FAILURE); //this string never must be executed
}

void FuncInstr::parseI( uint32 bytes)
{
    for ( unsigned i = 0; i < sizeof( isaTable) / sizeof( isaTable[0]); ++i)
    {
        if ( this->bytes.asI.op == isaTable[i].opcode
            && isaTable[i].format == FORMAT_I )
        {
            this->type = isaTable[i].type;
            this->cmd_name = ( char*)isaTable[i].name;
            return;
        }
    }
    cerr << "ERROR.Invalid I command" << endl;
    exit( EXIT_FAILURE); //this string never must be executed
}

string FuncInstr::Dump( string indent) const
{
    ostringstream os;
    os << indent << full_cmd_name;
    return os.str();
}

string FuncInstr::asReg( unsigned char reg) const
{
    switch( reg)
    {
        case ZERO: return "$zero";
        case AT: return "$at";
        case V0: return "$v0";
        case V1: return "$v1";
        case A0: return "$a0";
        case A1: return "$a1";
        case A2: return "$a2";
        case A3: return "$a3";
        case T0: return "$t0";
        case T1: return "$t1";
        case T2: return "$t2";
        case T3: return "$t3";
        case T4: return "$t4";
        case T5: return "$t5";
        case T6: return "$t6";
        case T7: return "$t7";
        case S0: return "$s0";
        case S1: return "$s1";
        case S2: return "$s2";
        case S3: return "$s3";
        case S4: return "$s4";
        case S5: return "$s5";
        case S6: return "$s6";
        case S7: return "$s7";
        case T8: return "$t8";
        case T9: return "$t9";
        case K0: return "$k0";
        case K1: return "$k1";
        case GP: return "$gp";
        case SP: return "$sp";
        case S8: return "$s8";
        case RA: return "$ra";
        default: assert( NULL); break;
    }
}

string FuncInstr::asRegOff( unsigned char reg, uint16 offset) const
{
    ostringstream os;

    os << "0x" << hex << offset << "(" << asReg( reg) << ")";
    return os.str();
}

FuncInstr::~FuncInstr()
{
    this->bytes.raw = NO_VAL32;
    this->format = NO_FORMAT;
    this->type = NO_TYPE;
    this->cmd_name = nullptr;
}

string FuncInstr::strFormat( FormatType format) const
{
    switch( format)
    {
        case FORMAT_R: return "R";
        case FORMAT_I: return "I";
        case FORMAT_J: return "J";
        default: assert(NULL);
    }
}

ostream& operator<< ( ostream& out, const FuncInstr& instr)
{
    out << instr.Dump( "");
    return out; 
}

bool FuncInstr::usesThreeRegOperands( Type type) const
{
    return ( type==ADD || type==ADDU || type==SUB || type==SUBU 
        || type==SLLV || type==SRLV|| type==SRAV || type==SLT
        || type==SLTU || type==AND || type==OR || type==XOR || type==NOR);
}

bool FuncInstr::usesTwoOperandsAndOffset( Type type) const
{
    return ( type==LB || type==LH || type==LW || type==LBU || type==LHU
            || type==SB || type==SH || type==SW);
}

bool FuncInstr::usesTwoRegOperandsAndOneImm( Type type) const
{
    return ( type==ADDI || type==ADDIU || type==SLL || type==SRL
            || type==SRA || type==SLTI || type==SLTIU
            || type==ANDI || type==ORI || type==XORI
            || type==BEQ || type==BNE );
}

bool FuncInstr::usesTwoRegOperands( Type type) const
{
    return ( type==MULT || type==MULTU || type==DIV || type==DIVU);
}

bool FuncInstr::usesOneRegOperand( Type type) const
{
    return ( type==MFHI || type==MTHI || type==MFLO || type==MTLO
            || type == JR || type==JALR);
}

bool FuncInstr::usesOneRegOperandAndOneImm( Type type) const
{
    return ( type==LUI || type==BLEZ || type==BGTZ);
}
