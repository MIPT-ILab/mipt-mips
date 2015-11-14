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

//uArchSim modules
#include <func_instr.h>

FuncInstr::FuncInstr( uint32 bytes_arg) 
{
    this->bytes.raw = bytes_arg;
    this->initFormat(bytes.raw);
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
            assert(NULL);
            break;
    }
}

void FuncInstr::initFormat( uint32 bytes)
{
    uint32 last6 = bytes % ( 1U << 6U);
    if ( last6 == 0)
    {
        this->format=FORMAT_R;
    }
    else if ( last6 == 2 || last6 == 3 || last6 == 0x1A)
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
    for ( unsigned i = 0; i < sizeof( isaTable) / sizeof( isaTable[0]); ++i)
    {
        if ( this->bytes.asR.funct == isaTable[i].func
                && isaTable[i].format == FORMAT_R )
        {
            this->type = isaTable[i].type;
            strcpy( this->cmd_name, isaTable[i].name);
            return;
        }
    }
    assert(NULL); //this string never must be executed
}

void FuncInstr::parseJ( uint32 bytes)
{
    for ( unsigned i = 0; i < sizeof( isaTable) / sizeof( isaTable[0]); ++i)
    {
        if ( this->bytes.asJ.op == isaTable[i].opcode
            && isaTable[i].format == FORMAT_J )
        {
            this->type = isaTable[i].type;
            strcpy( this->cmd_name, isaTable[i].name);
            return;
        }
    }
    assert(NULL); //this string never must be executed
}

void FuncInstr::parseI( uint32 bytes)
{
    for ( unsigned i = 0; i < sizeof( isaTable) / sizeof( isaTable[0]); ++i)
    {
        if ( this->bytes.asI.op == isaTable[i].opcode
            && isaTable[i].format == FORMAT_I )
        {
            this->type = isaTable[i].type;
            strcpy( this->cmd_name, isaTable[i].name);
            return;
        }
    }
    assert(NULL); //this string never must be executed
}

string FuncInstr::Dump( string indent) const
{
    ostringstream os;

    os << indent << this->cmd_name;
    switch ( this->format)
    {
        case FORMAT_J:
            os << hex << this->bytes.asJ.offset;
            break;
        case FORMAT_I:
            if ( isLoadStore( this->type))
            {
                os << asReg( this->bytes.asI.rt) << ", " 
                   << asRegOff( this->bytes.asI.rs, this->bytes.asI.imm) ;
            }
            else if ( isAluOp( this->type))
            {
                os << asReg( this->bytes.asI.rs) << ", "
                   << asReg( this->bytes.asI.rt) << ", "
                   << this->bytes.asI.imm;
            }
            else
            {
                assert( NULL);
            }
            break;
        case FORMAT_R:
            os << asReg( this->bytes.asR.rs) << ", "
               << asReg( this->bytes.asR.rt) << ", "
               << asReg( this->bytes.asR.rd);
            break;
        default:
            assert( NULL);
            break;
    }

    return os.str();
}

bool FuncInstr::isLoadStore( Type type) const
{
    return ( type==LB || type == LH || type == LW || type == LBU
        || type==LHU || type==SB || type==SH || type==SW);
}

bool FuncInstr::isAluOp( Type type) const
{
    return ( type==ADD || type==ADDU || type==SUB || type==SUBU
        || type==ADDI || type==ADDIU || type==MULT || type==MULTU
        || type==DIV || type==DIVU);
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
